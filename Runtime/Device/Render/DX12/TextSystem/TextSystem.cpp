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

std::vector<DrawUIItem> TextSystem::BuildDrawItems(std::span<const DrawTextItem> items)
{
    std::vector<DrawUIItem> result;
    if (items.empty())
        return result;

    std::vector<ShapedText> shapedTexts;
    shapedTexts.reserve(items.size());

    std::vector<GlyphUploadEntry> uploads; // 이번 프레임에 새롭게 아틀라스에 추가할 글자들의 목록
    for (const auto& item : items)
    {
        auto font = static_cast<FontResource*>(item.fontRes.get());

        ShapedText shaped;
        shaped.font = font;
        shaped.size = item.fontSize;
        shaped.glyphs = font->Shape(item.codePoints, item.fontSize);

        UpdateAtlasIfNeeded(shaped, uploads);
        shapedTexts.push_back(std::move(shaped));
    }

    if (!uploads.empty())
        m_atlasBuilder.UploadGlyphsToAtlas(uploads);

    auto mesh = CreateTextMesh(items, shapedTexts);
    if (!mesh)
        return result;

    DrawUIItem uiItem;
    uiItem.mesh = std::move(mesh);
    uiItem.material = m_matResource;

    result.push_back(std::move(uiItem));
    return result;
}

std::shared_ptr<TransientMeshResource> TextSystem::CreateTextMesh(
    std::span<const DrawTextItem> items,
    std::span<const ShapedText> shapedTexts)
{
    size_t totalGlyphCount = 0;
    for (const auto& shapedText : shapedTexts)
        totalGlyphCount += shapedText.glyphs.size();
    if (totalGlyphCount == 0)
        return nullptr;

    std::vector<UIVertex> vertices;
    std::vector<uint32_t> indices;
    vertices.reserve(totalGlyphCount * 4);
    indices.reserve(totalGlyphCount * 6);

    uint32_t vertexOffset = 0;
    for (size_t itemIndex = 0; itemIndex < items.size(); ++itemIndex)
    {
        const auto& item = items[itemIndex];
        const auto& shapedText = shapedTexts[itemIndex];
        auto font = shapedText.font;

        float cursorX = item.position.x;
        float baselineY = item.position.y;
        for (const auto& shapedGlyph : shapedText.glyphs)
        {
            const GlyphInfo* glyph = m_glyphCache.Get(font, shapedGlyph.glyphIndex, shapedText.size);
            if (!glyph)
                continue;

            float x0 = cursorX + glyph->bearingX + shapedGlyph.offsetX;
            float y0 = baselineY - glyph->bearingY - shapedGlyph.offsetY;
            float x1 = x0 + glyph->width;
            float y1 = y0 + glyph->height;

            vertices.push_back({ { x0, y0, 0.f }, item.color, { glyph->uvMin.x, glyph->uvMin.y } });
            vertices.push_back({ { x1, y0, 0.f }, item.color, { glyph->uvMax.x, glyph->uvMin.y } });
            vertices.push_back({ { x1, y1, 0.f }, item.color, { glyph->uvMax.x, glyph->uvMax.y } });
            vertices.push_back({ { x0, y1, 0.f }, item.color, { glyph->uvMin.x, glyph->uvMax.y } });

            indices.insert(indices.end(), {
                vertexOffset + 0, vertexOffset + 1, vertexOffset + 2,
                vertexOffset + 0, vertexOffset + 2, vertexOffset + 3
                });

            vertexOffset += 4;
            cursorX += shapedGlyph.advanceX; // HarfBuzz가 계산한 위치 이동량
        }
    }
    if (vertices.empty())
        return nullptr;

    return m_transientMeshProvider.Create(vertices, indices);
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
    const ShapedText& shapedText,
    std::vector<GlyphUploadEntry>& outUploads)
{
    auto font = shapedText.font;

    for (const auto& glyph : shapedText.glyphs)
    {
        uint32_t glyphIndex = glyph.glyphIndex;
        if (m_glyphCache.Contains(font, glyphIndex, shapedText.size))
            continue;

        FT_GlyphSlot slot = font->GetGlyphSlotByGlyphIndex(glyphIndex, shapedText.size);
        Assert(slot);

        uint32_t width = slot->bitmap.width;
        uint32_t height = slot->bitmap.rows;

        auto [packX, packY] = m_packer.AllocateRect(width + 2, height + 2);

        GlyphUploadEntry uploadEntry{ GetAtlasResource() };
        if (CreateUploadEntry(slot, packX, packY, uploadEntry))
            outUploads.push_back(std::move(uploadEntry));

        GlyphInfo glyphInfo;
        glyphInfo.width = static_cast<float>(width);
        glyphInfo.height = static_cast<float>(height);
        glyphInfo.bearingX = static_cast<float>(slot->bitmap_left);
        glyphInfo.bearingY = static_cast<float>(slot->bitmap_top);

        float atlasW = static_cast<float>(m_atlasTextureSize.width);
        float atlasH = static_cast<float>(m_atlasTextureSize.height);

        glyphInfo.uvMin = { static_cast<float>(packX) / atlasW, static_cast<float>(packY) / atlasH };
        glyphInfo.uvMax = { static_cast<float>(packX + width) / atlasW, static_cast<float>(packY + height) / atlasH };

        m_glyphCache.Insert(font, glyphIndex, shapedText.size, glyphInfo);
    }
}

const Resource& TextSystem::GetAtlasResource() const
{
    auto tex = m_matResource->GetTexture(Resolve(UITextureSlot::Normal));
    return tex->Get();
}