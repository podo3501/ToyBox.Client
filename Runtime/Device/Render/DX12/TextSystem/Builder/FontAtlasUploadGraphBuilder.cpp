#include "pch.h"
#include "FontAtlasUploadGraphBuilder.h"
#include "Factory/ResourceFactory.h"
#include "Graph/RenderGraph.h"
#include "Graph/TaskScheduler.h"
#include "Core/Foundation/Align.h"
#include "RenderConstants.h"
#include "Command/CommandList.h"

struct TextRenderLayout
{
    size_t offset{ 0 };
    size_t rowPitch{ 0 };
};

static DXGI_FORMAT GetDXGIFormat(GlyphPixelFormat format)
{
    switch (format)
    {
    case GlyphPixelFormat::R8: return DXGI_FORMAT_R8_UNORM;
    case GlyphPixelFormat::RG8: return DXGI_FORMAT_R8G8_UNORM;
    case GlyphPixelFormat::RGBA8: return DXGI_FORMAT_R8G8B8A8_UNORM;
    }

    Assert(false);
    return DXGI_FORMAT_UNKNOWN;
}

FontAtlasUploadGraphBuilder::~FontAtlasUploadGraphBuilder() = default;
FontAtlasUploadGraphBuilder::FontAtlasUploadGraphBuilder(
    TaskScheduler& taskScheduler,
    ResourceFactory& resFactory) :
    m_taskScheduler{ taskScheduler },
    m_resFactory{ resFactory }
{}

void FontAtlasUploadGraphBuilder::UploadGlyphsToAtlas(
    const Resource& atlasResource,
    const std::vector<GlyphUploadEntry>& uploads)
{
    std::vector<TextRenderLayout> layouts;

    size_t uploadOffset = 0;
    for (const auto& glyph : uploads)
    {
        uint32_t bytesPerPixel = GetBytesPerPixel(glyph.bitmap.format);

        size_t bytesPerRow = glyph.bitmap.width * bytesPerPixel;
        size_t rowPitch = Core::AlignUp(bytesPerRow, AlignTextureRow);
        size_t glyphSize = rowPitch * glyph.bitmap.height;

        uploadOffset = Core::AlignUp(uploadOffset, AlignTexturePlacement);
        layouts.push_back({ uploadOffset, rowPitch });

        uploadOffset += glyphSize;
    }
    size_t totalUploadSize = uploadOffset;

    RenderGraph graph;
    RGResourceID atlasResID = RenderGraph::CreateRGResourceID();
    RGResourceID uploadResID = RenderGraph::CreateRGResourceID();

    graph.ImportResource(atlasResID, RGAccess::SRV);

    BuildUploadPass(graph, atlasResID, uploadResID, uploads, layouts);

    graph.ExportResource(atlasResID, RGAccess::SRV);

    auto compiledTasks = graph.Compile();

    auto resCtx = std::make_shared<ResourceContext>();
    resCtx->Set(atlasResID, atlasResource);
    resCtx->Set(uploadResID, m_resFactory.CreateResource(totalUploadSize, ResInitType::Upload));
    m_taskScheduler.SubmitTask(compiledTasks, resCtx);
}

static void CopyGlyphToUploadBuffer(
    uint8_t* mapped,
    const GlyphUploadEntry& glyph,
    const TextRenderLayout& layout)
{
    uint32_t bytesPerPixel = GetBytesPerPixel(glyph.bitmap.format);
    uint32_t bytesPerRow = glyph.bitmap.width * bytesPerPixel;

    for (uint32_t y = 0; y < glyph.bitmap.height; ++y)
    {
        std::memcpy(
            mapped + layout.offset + y * layout.rowPitch,
            glyph.bitmap.pixels.data() + y * bytesPerRow,
            bytesPerRow);
    }
}

static void CopyGlyphToAtlas(
    CommandList& cmd,
    Resource& uploadBuffer,
    Resource& atlasTexture,
    const GlyphUploadEntry& glyph,
    const TextRenderLayout& layout)
{
    D3D12_TEXTURE_COPY_LOCATION dst{};
    dst.pResource = atlasTexture.Get();
    dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dst.SubresourceIndex = 0;

    D3D12_TEXTURE_COPY_LOCATION src{};
    src.pResource = uploadBuffer.Get();
    src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    src.PlacedFootprint.Offset = layout.offset;
    src.PlacedFootprint.Footprint.Format = GetDXGIFormat(glyph.bitmap.format);
    src.PlacedFootprint.Footprint.Width = glyph.bitmap.width;
    src.PlacedFootprint.Footprint.Height = glyph.bitmap.height;
    src.PlacedFootprint.Footprint.Depth = 1;
    src.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(layout.rowPitch);

    cmd->CopyTextureRegion(
        &dst,
        glyph.x,
        glyph.y,
        0,
        &src,
        nullptr);
}

void FontAtlasUploadGraphBuilder::BuildUploadPass(
    RenderGraph& graph,
    RGResourceID atlasResID,
    RGResourceID uploadResID,
    const std::vector<GlyphUploadEntry>& uploads,
    const std::vector<TextRenderLayout>& layouts)
{
    auto& upload = graph.AddCopyPass("FontAtlasUpload");
    upload.Write(atlasResID, RGAccess::CopyDest);

    upload.gpuExecute =
        [
            atlasResID,
            uploadResID,
            uploads,
            layouts
        ]
        (CommandList& cmd, TaskContext& ctx)
        {
            auto& uploadBuffer = ctx.GetResource(uploadResID);
            auto& atlasResource = ctx.GetResource(atlasResID);

            uint8_t* pCpuMem = nullptr;
            uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&pCpuMem));

            for (size_t i = 0; i < uploads.size(); ++i)
            {
                CopyGlyphToUploadBuffer(
                    pCpuMem,
                    uploads[i],
                    layouts[i]);

                CopyGlyphToAtlas(
                    cmd,
                    uploadBuffer,
                    atlasResource,
                    uploads[i],
                    layouts[i]);
            }

            uploadBuffer->Unmap(0, nullptr);
        };
}
