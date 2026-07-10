#include "pch.h"
#include "TextSystem.h"
#include "Core/Device.h"
#include "Core/Utils/StringUtils.h"
#include "Core/Math/Matrix.h"
#include "Resource/Font/FontResource.h"
#include "Resource/Texture/TextureResource.h"
#include "Factory/DescriptorFactory.h"
#include "Factory/ResourceFactory.h"
#include "Graph/RenderGraph.h"
#include "Graph/TaskScheduler.h"
#include "Helpers/TextureHelpers.h"
#include "Helpers/CommonHelpers.h"
#include "Command/CommandList.h"

struct GlyphUploadEntry // 하나의 글자를 아틀라스로 전송하기 위한 데이터 묶음
{
    std::vector<uint8_t> pixelData; // CPU 비트맵 임시 버퍼
    uint32_t width;
    uint32_t height;
    uint32_t packX;
    uint32_t packY;
};

namespace cm = Core::Math;

static Resource CreateFontAtlasResource(Device& device, const Size& atlasSize)
{
    auto desc = CreateTextureDescriptor(atlasSize.width, atlasSize.height, DXGI_FORMAT_R8_UNORM); // 글자 비트맵(Alpha) 정보만 담으면 되므로 R8_UNORM 포맷이 가장 효율적
    desc.Flags |= D3D12_RESOURCE_FLAG_NONE;

    return device.CreateResource(
        desc,
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        nullptr);
}

TextSystem::~TextSystem() = default;
TextSystem::TextSystem(TaskScheduler& taskScheduler, ResourceFactory& resFactory) :
    m_taskScheduler{ taskScheduler },
    m_resFactory{ resFactory }
{}

bool TextSystem::Initialize(
    Device& device, 
    DescriptorFactory& factory, 
    const Size& atlasTexSize)
{
    auto resource = CreateFontAtlasResource(device, atlasTexSize);
    if (!resource) return false;

    // 셰이더에서 이 아틀라스를 바인딩해서 글자를 그릴 수 있도록 SRV만 생성합니다.
    UINT srvIndex = factory.CreateTextureSRV(resource, DXGI_FORMAT_R8_UNORM);
    if (srvIndex == UINT_MAX)
        return false;

    ReturnIfFalse(m_packer.Initialize(atlasTexSize));
    m_atlasTextureSize = atlasTexSize;

    TextureDesc atlasTexDesc
    {
        .resID = Core::ResourceID::MakeBuiltin("atlas"),
        .type = TextureType::Color,
        .generateMips = false
    };
    auto atlasTex = make_shared<TextureResource>(atlasTexDesc);
    atlasTex->Set(std::move(resource));
    atlasTex->SetHeapIndex(srvIndex);
    atlasTex->SetSize(atlasTexSize);
    atlasTex->MarkReady();

    UIMaterialDesc uiTextMaterialDesc;
    uiTextMaterialDesc.SetShaderID(RegistryShader::Text);
    auto matRes = make_shared<UIMaterialResource>(uiTextMaterialDesc);
    matRes->SetTexture(Resolve(UITextureSlot::Normal), atlasTex);
    m_matResource = matRes;

    return true;
}

std::vector<DrawUIItem> TextSystem::DrawText(
    std::shared_ptr<IMeshResource> meshRes,
    std::shared_ptr<IFontResource> fontRes,
    std::string_view text,
    uint32_t size,
    const Core::Math::Vector2& pos)
{
    std::vector<DrawUIItem> result;
    if (text.empty()) return result;

    UpdateAtlasIfNeeded(fontRes, text, size);
    
    auto font = static_cast<FontResource*>(fontRes.get());

    float cursorX = pos.x;
    float baselineY = pos.y;

    size_t offset = 0;
    while (offset < text.size())
    {
        char32_t codepoint = Core::UTF8ToUTF32Char(text, offset);
        if (codepoint == U'\0')
            continue;

        const GlyphInfo* glyph = m_glyphCache.Get(font, codepoint, size);
        if (!glyph)
            continue;

        DrawUIItem item;
        item.mesh = meshRes;
        item.material = m_matResource;

        float drawX = cursorX + glyph->bearingX;
        float drawY = baselineY - glyph->bearingY;

        item.world =
            cm::Matrix::Scale(
                glyph->width,
                glyph->height,
                1.0f) *
            cm::Matrix::Translation(
                drawX,
                drawY,
                0.0f);

        item.uvTransform = cm::Vector4{
            glyph->uvMin.x,
            glyph->uvMin.y,
            glyph->uvMax.x,
            glyph->uvMax.y
        };

        result.push_back(std::move(item));

        cursorX += glyph->advanceX;
    }

    return result;
}

static bool CreateUploadEntry(
    FT_GlyphSlot slot, 
    uint32_t packX, 
    uint32_t packY,
    GlyphUploadEntry& outEntry)
{
    uint32_t width = slot->bitmap.width;
    uint32_t height = slot->bitmap.rows;

    if (width == 0 || height == 0)
        return false;

    outEntry.width = width;
    outEntry.height = height;
    outEntry.packX = packX;
    outEntry.packY = packY;

    outEntry.pixelData.resize(width * height);

    for (uint32_t y = 0; y < height; ++y)
    {
        std::memcpy(
            &outEntry.pixelData[y * width],
            &slot->bitmap.buffer[y * slot->bitmap.pitch],
            width);
    }

    return true;
}

void TextSystem::UpdateAtlasIfNeeded(
    std::shared_ptr<IFontResource> fontResource, 
    std::string_view text,
    uint32_t size)
{
    if (text.empty()) return;
    auto font = static_cast<FontResource*>(fontResource.get());

    std::vector<GlyphUploadEntry> pendingUploads; // 이번 프레임에 새롭게 아틀라스에 추가할 글자들의 목록

    size_t offset = 0;
    while (offset < text.size())
    {
        char32_t codepoint = Core::UTF8ToUTF32Char(text, offset);
        if (codepoint == U'\0')
            continue;
        if (m_glyphCache.Contains(font, codepoint, size))
            continue;

        FT_GlyphSlot slot = font->GetGlyphSlot(codepoint, size);
        Assert(slot);

        uint32_t width = slot->bitmap.width;
        uint32_t height = slot->bitmap.rows;

        auto [packX, packY] = m_packer.AllocateRect(width + 2, height + 2);

        GlyphUploadEntry uploadEntry;
        if (CreateUploadEntry(slot, packX, packY, uploadEntry))
            pendingUploads.push_back(std::move(uploadEntry));

        GlyphInfo glyphInfo;
        glyphInfo.width = static_cast<float>(width);
        glyphInfo.height = static_cast<float>(height);
        glyphInfo.bearingX = static_cast<float>(slot->bitmap_left);
        glyphInfo.bearingY = static_cast<float>(slot->bitmap_top);
        glyphInfo.advanceX = static_cast<float>(slot->advance.x >> 6); // 26.6 고정소수점 변환

        // 아틀라스 텍스처 해상도 기준의 0.0 ~ 1.0 UV 좌표 계산
        float atlasW = static_cast<float>(m_atlasTextureSize.width);
        float atlasH = static_cast<float>(m_atlasTextureSize.height);

        glyphInfo.uvMin = cm::Vector2(static_cast<float>(packX) / atlasW, static_cast<float>(packY) / atlasH);
        glyphInfo.uvMax = cm::Vector2(static_cast<float>(packX + width) / atlasW, static_cast<float>(packY + height) / atlasH);

        m_glyphCache.Insert(font, codepoint, size, glyphInfo);
    }

    if (!pendingUploads.empty())
        UploadGlyphsToAtlas(pendingUploads);
}

struct TextRenderLayout 
{ 
    size_t offset{ 0 };
    size_t rowPitch{ 0 };
};

void TextSystem::UploadGlyphsToAtlas(const std::vector<GlyphUploadEntry>& uploads)
{
    std::vector<TextRenderLayout> layouts;

    size_t uploadOffset = 0;
    for (const auto& glyph : uploads)
    {
        size_t rowPitch = AlignSize(glyph.width, AlignTextureRow);
        size_t glyphSize = rowPitch * glyph.height;

        uploadOffset = AlignSize(uploadOffset, AlignTexture);
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
    resCtx->Set(atlasResID, GetAtlasResource());
    resCtx->Set(uploadResID, m_resFactory.CreateResource(totalUploadSize, ResInitType::Upload));
    m_taskScheduler.SubmitTask(compiledTasks, resCtx);
}

static void CopyGlyphToUploadBuffer(
    uint8_t* mapped,
    const GlyphUploadEntry& glyph,
    const TextRenderLayout& layout)
{
    for (uint32_t y = 0; y < glyph.height; ++y)
    {
        std::memcpy(
            mapped + layout.offset + y * layout.rowPitch,
            glyph.pixelData.data() + y * glyph.width,
            glyph.width);
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
    src.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8_UNORM;
    src.PlacedFootprint.Footprint.Width = glyph.width;
    src.PlacedFootprint.Footprint.Height = glyph.height;
    src.PlacedFootprint.Footprint.Depth = 1;
    src.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(layout.rowPitch);

    cmd->CopyTextureRegion(
        &dst,
        glyph.packX,
        glyph.packY,
        0,
        &src,
        nullptr);
}

void TextSystem::BuildUploadPass(
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

const Resource& TextSystem::GetAtlasResource() const
{
    auto tex = m_matResource->GetTexture(Resolve(UITextureSlot::Normal));
    return tex->Get();
}