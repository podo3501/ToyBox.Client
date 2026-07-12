#include "pch.h"
#include "TextSystem.h"
#include "Core/Device.h"
#include "Core/Utils/StringUtils.h"
#include "Resource/Font/FontResource.h"
#include "Resource/Texture/TextureResource.h"
#include "Resource/Mesh/TransientMeshResource.h"
#include "Factory/DescriptorFactory.h"
#include "Helpers/TextureHelpers.h"
#include "GameClient/Asset/MeshAsset.h"
#include "Provider/Mesh/TransientMeshProvider.h"

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
TextSystem::TextSystem(
    TaskScheduler& taskScheduler, 
    ResourceFactory& resFactory,
    TransientMeshProvider& transientMeshProvider) :
    m_atlasBuilder{ taskScheduler, resFactory },
    m_transientMeshProvider{ transientMeshProvider }
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
    std::shared_ptr<IFontResource> fontRes,
    std::span<const char32_t> text,
    uint32_t size,
    const Core::Vector2& pos,
    const Core::Color& color)
{
    std::vector<DrawUIItem> result;
    if (text.empty()) return result;

    UpdateAtlasIfNeeded(fontRes, text, size);
    auto font = static_cast<FontResource*>(fontRes.get());

    std::vector<UIVertex> vertices;
    std::vector<uint32_t> indices;

    vertices.reserve(text.size() * 4);
    indices.reserve(text.size() * 6);

    float cursorX = pos.x;
    float baselineY = pos.y;
    uint32_t vertexOffset = 0;

    for (auto codepoint : text)
    {
        const GlyphInfo* glyph = m_glyphCache.Get(font, codepoint, size);
        if (!glyph) continue;

        float x0 = cursorX + glyph->bearingX;
        float y0 = baselineY - glyph->bearingY;
        float x1 = x0 + glyph->width;
        float y1 = y0 + glyph->height;

        vertices.push_back({ {x0, y0, 0.f}, color, {glyph->uvMin.x, glyph->uvMin.y} }); //0
        vertices.push_back({ {x1, y0, 0.f}, color, {glyph->uvMax.x, glyph->uvMin.y} }); // 1
        vertices.push_back({ {x1, y1, 0.f}, color, {glyph->uvMax.x, glyph->uvMax.y} }); // 2
        vertices.push_back({ {x0, y1, 0.f}, color, {glyph->uvMin.x, glyph->uvMax.y} }); // 3

        indices.insert(indices.end(), {
            vertexOffset + 0, vertexOffset + 1, vertexOffset + 2,
            vertexOffset + 0, vertexOffset + 2, vertexOffset + 3
            });

        vertexOffset += 4;
        cursorX += glyph->advanceX;
    }

    auto mesh = m_transientMeshProvider.Create(vertices, indices);
    if (!mesh)
        return result;

    DrawUIItem item;

    item.mesh = mesh;
    item.material = m_matResource;

    result.push_back(std::move(item)); //지금은 하나만 넣는데 나중에 텍스쳐가 여러장이거나, 머터리얼 변경이 있을수도 있음.
    return result;
}


//std::vector<DrawUIItem> TextSystem::DrawText(
//    std::shared_ptr<IMeshResource> meshRes,
//    std::shared_ptr<IFontResource> fontRes,
//    std::span<const char32_t> text,
//    uint32_t size,
//    const Core::Vector2& pos,
//    const Core::Color& color)
//{
//    std::vector<DrawUIItem> result;
//    if (text.empty()) return result;
//
//    UpdateAtlasIfNeeded(fontRes, text, size);
//    
//    auto font = static_cast<FontResource*>(fontRes.get());
//
//    float cursorX = pos.x;
//    float baselineY = pos.y;
//
//    for (auto codepoint : text)
//    {
//        const GlyphInfo* glyph = m_glyphCache.Get(font, codepoint, size);
//        if (!glyph)
//            continue;
//
//        DrawUIItem item;
//        item.mesh = meshRes;
//        item.material = m_matResource;
//
//        float drawX = cursorX + glyph->bearingX;
//        float drawY = baselineY - glyph->bearingY;
//
//        item.world =
//            Core::Matrix::Scale(glyph->width, glyph->height, 1.0f) *
//            Core::Matrix::Translation(drawX, drawY, 0.0f);
//
//        item.uvTransform = Core::Vector4{
//            glyph->uvMin.x,
//            glyph->uvMin.y,
//            glyph->uvMax.x,
//            glyph->uvMax.y
//        };
//
//        result.push_back(std::move(item));
//
//        cursorX += glyph->advanceX;
//    }
//
//    return result;
//}

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
    std::span<const char32_t> text,
    uint32_t size)
{
    if (text.empty()) return;
    auto font = static_cast<FontResource*>(fontResource.get());

    std::vector<GlyphUploadEntry> pendingUploads; // 이번 프레임에 새롭게 아틀라스에 추가할 글자들의 목록

    for (auto codepoint : text)
    {
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

        glyphInfo.uvMin = Core::Vector2(static_cast<float>(packX) / atlasW, static_cast<float>(packY) / atlasH);
        glyphInfo.uvMax = Core::Vector2(static_cast<float>(packX + width) / atlasW, static_cast<float>(packY + height) / atlasH);

        m_glyphCache.Insert(font, codepoint, size, glyphInfo);
    }

    if (!pendingUploads.empty())
        m_atlasBuilder.UploadGlyphsToAtlas(GetAtlasResource(), pendingUploads);
}

const Resource& TextSystem::GetAtlasResource() const
{
    auto tex = m_matResource->GetTexture(Resolve(UITextureSlot::Normal));
    return tex->Get();
}