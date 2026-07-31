#include "pch.h"
#include "FontAtlasUploadGraphBuilder.h"
#include "Factory/ResourceFactory.h"
#include "Graph/RenderGraph.h"
#include "Graph/TaskScheduler.h"
#include "Core/Foundation/Align.h"
#include "RenderConstants.h"
#include "Command/CommandList.h"

struct GlyphUploadLayout
{
    size_t offset{ 0 };
    size_t rowPitch{ 0 };
    size_t bytesPerRow{ 0 };
};

FontAtlasUploadGraphBuilder::~FontAtlasUploadGraphBuilder() = default;
FontAtlasUploadGraphBuilder::FontAtlasUploadGraphBuilder(
    TaskScheduler& taskScheduler,
    ResourceFactory& resFactory) :
    m_taskScheduler{ taskScheduler },
    m_resFactory{ resFactory }
{}

void FontAtlasUploadGraphBuilder::UploadGlyphsToAtlas(
    const Resource& atlasResource,
    std::vector<GlyphUploadEntry> uploads)
{
    if (uploads.empty())
        return; 

    std::vector<GlyphUploadLayout> layouts;

    size_t uploadOffset = 0;
    for (const auto& glyph : uploads)
    {
        uint32_t bytesPerPixel = GetBytesPerPixel(glyph.pixels.format);

        size_t bytesPerRow = static_cast<size_t>(glyph.pixels.width) * bytesPerPixel;
        size_t rowPitch = Core::AlignUp(bytesPerRow, AlignTextureRow);
        size_t glyphSize = rowPitch * glyph.pixels.height;

        uploadOffset = Core::AlignUp(uploadOffset, AlignTexturePlacement);
        layouts.push_back({ uploadOffset, rowPitch, bytesPerRow });

        uploadOffset += glyphSize;
    }
    size_t totalUploadSize = uploadOffset;

    RenderGraph graph;
    RGResourceID atlasResID = RenderGraph::CreateRGResourceID();
    RGResourceID uploadResID = RenderGraph::CreateRGResourceID();

    graph.ImportResource(atlasResID, RGAccess::SRV);

    BuildUploadPass(graph, atlasResID, uploadResID, std::move(uploads), std::move(layouts));

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
    const GlyphUploadLayout& layout)
{
    for (uint32_t y = 0; y < glyph.pixels.height; ++y)
    {
        std::memcpy(
            mapped + layout.offset + y * layout.rowPitch,
            glyph.pixels.buffer.data() + y * layout.bytesPerRow,
            layout.bytesPerRow);
    }
}

static void CopyGlyphToAtlas(
    CommandList& cmd,
    Resource& uploadBuffer,
    Resource& atlasTexture,
    const GlyphUploadEntry& glyph,
    const GlyphUploadLayout& layout)
{
    D3D12_TEXTURE_COPY_LOCATION dst{};
    dst.pResource = atlasTexture.Get();
    dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    dst.SubresourceIndex = 0;

    D3D12_TEXTURE_COPY_LOCATION src{};
    src.pResource = uploadBuffer.Get();
    src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    src.PlacedFootprint.Offset = layout.offset;
    src.PlacedFootprint.Footprint.Format = GetDXGIFormat(glyph.pixels.format);
    src.PlacedFootprint.Footprint.Width = glyph.pixels.width;
    src.PlacedFootprint.Footprint.Height = glyph.pixels.height;
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
    std::vector<GlyphUploadEntry> uploads,
    std::vector<GlyphUploadLayout> layouts)
{
    auto& upload = graph.AddCopyPass("FontAtlasUpload");
    upload.Write(atlasResID, RGAccess::CopyDest);

    upload.gpuExecute =
        [
            atlasResID,
            uploadResID,
            uploads = std::move(uploads),
            layouts = std::move(layouts)
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
