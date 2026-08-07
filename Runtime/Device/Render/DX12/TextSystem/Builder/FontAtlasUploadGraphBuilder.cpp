#include "pch.h"
#include "FontAtlasUploadGraphBuilder.h"
#include "Factory/ResourceFactory.h"
#include "Graph/RenderGraph.h"
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
FontAtlasUploadGraphBuilder::FontAtlasUploadGraphBuilder(ResourceFactory& resFactory) :
    m_resFactory{ resFactory }
{}

void FontAtlasUploadGraphBuilder::QueueGlyphUploads(std::vector<AtlasGlyphBatch>&& batches)
{
    Assert(m_pending.empty()); // 프레임에 한번씩만 들어오거나 안들어온다(새로운글자가 있을때만). Build에서 소비하는데 소비하지 못했다면 문제.
    for (auto& batch : batches)
    {
        m_pending.emplace(
            batch.atlasResource,
            PendingAtlas{ RenderGraph::CreateRGResourceID(), std::move(batch.glyphs) });
    }
}

void FontAtlasUploadGraphBuilder::Build(RenderGraph& graph)
{
    m_frameBindings.clear();

    if (m_pending.empty())
        return;

    for (auto& [atlasResPtr, pending] : m_pending)
    {
        std::vector<GlyphUploadLayout> layouts;
        layouts.reserve(pending.glyphs.size());

        size_t uploadOffset = 0;
        for (const auto& glyph : pending.glyphs)
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

        RGResourceID uploadResID = RenderGraph::CreateRGResourceID();

        graph.ImportResource(pending.rgID, RGAccess::SRV);
        BuildUploadPass(graph, pending.rgID, uploadResID, std::move(pending.glyphs), std::move(layouts));
        graph.ExportResource(pending.rgID, RGAccess::SRV);

        // atlas(원본 리소스)와 새로 만든 upload staging 리소스를 이번 프레임 바인딩 목록에 기록
        m_frameBindings.emplace(pending.rgID, *atlasResPtr);
        m_frameBindings.emplace(uploadResID, m_resFactory.CreateResource(totalUploadSize, ResInitType::Upload));
    }

    m_pending.clear();
}

void FontAtlasUploadGraphBuilder::ApplyResourceBindings(ResourceContext& resCtx) const
{
    for (auto& [id, resource] : m_frameBindings)
        resCtx.Set(id, resource);
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
    auto& upload = graph.AddGraphicsPass("FontAtlasUpload");
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