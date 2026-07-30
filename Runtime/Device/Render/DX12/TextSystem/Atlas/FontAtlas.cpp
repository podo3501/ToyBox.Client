#include "pch.h"
#include "FontAtlas.h"
#include "Glyph/BitmapGlyphGenerator.h"
#include "../Builder/FontAtlasUploadGraphBuilder.h"
#include "BitmapFontAtlasBucket.h"
#include "MTSDFFontAtlasBucket.h"

struct FontAtlasUpload
{
    std::vector<std::vector<GlyphUploadEntry>> pages;
};

FontAtlas::~FontAtlas() = default;
FontAtlas::FontAtlas(
    Device& device, 
    DescriptorFactory& factory, 
    FontAtlasUploadGraphBuilder& atlasBuilder) :
    m_device{ device },
    m_factory{ factory },
    m_atlasBuilder{ atlasBuilder }
{}

bool FontAtlas::Initialize(const TextConfig& config)
{
    m_textConfig = config;
    EnsureSolidPage();

    return true;
}

void FontAtlas::EnsureSolidPage()
{
    AtlasPageDesc solidDesc{};
    solidDesc.format = GlyphPixelFormat::RGBA8; //알파 포함 RGBA 필요
    solidDesc.shaderID = RegistryShader::UI;

    constexpr Size kSolidPageSize{ 4, 4 };
    m_solidPage.Initialize(m_device, m_factory, kSolidPageSize, solidDesc);

    // 4x4를 전부 흰색(premultiplied RGBA = 255,255,255,255)으로 채워 1회 업로드
    GlyphPixels pixels{ 
        .format = GlyphPixelFormat::RGBA8, 
        .width = 4, 
        .height = 4 
    };

    pixels.buffer.resize(pixels.width * pixels.height * 4);
    std::fill(pixels.buffer.begin(), pixels.buffer.end(), uint8_t{ 0xFF });

    GlyphUploadEntry entry;
    entry.x = 0;
    entry.y = 0;
    entry.pixels = std::move(pixels);

    m_atlasBuilder.UploadGlyphsToAtlas(m_solidPage.GetAtlasResource(), { entry });
}

void FontAtlas::EnsureGlyphs(std::span<const ShapedText> shapedTexts)
{
    std::unordered_map<FontBucketKey, FontAtlasUpload, FontBucketKeyHash> uploads;

    for (const auto& shapedText : shapedTexts)
    {
        FontBucketID bucket = GetFontBucketID(shapedText.mode, shapedText.size);

        FontBucketKey key{ shapedText.mode, bucket };
        auto& upload = uploads[key];

        auto atlasBucket = GetOrCreateBucket(key);
        atlasBucket->EnsureGlyphs(
            shapedText,
            upload.pages);
    }

    for (auto& [key, upload] : uploads)
    {
        for (uint16_t page = 0; page < upload.pages.size(); ++page)
        {
            auto& glyphs = upload.pages[page];
            if (glyphs.empty())
                continue;

            auto& atlasRes = GetAtlasResource(key, page);
            m_atlasBuilder.UploadGlyphsToAtlas(atlasRes, glyphs);
        }
    }
}

const GlyphInfo* FontAtlas::FindGlyph(
    FontResource* font,
    TextRenderMode mode,
    uint32_t glyphIndex,
    uint32_t size) const
{
    FontBucketID bucket = GetFontBucketID(mode, size);

    FontBucketKey key{ mode, bucket };
    auto atlasBucket = FindBucket(key);
    if (!atlasBucket)
        return nullptr;
    
    return atlasBucket->FindGlyph(
        font,
        glyphIndex,
        size);
}

std::shared_ptr<IMaterialResource> FontAtlas::GetMaterial(const GlyphInfo* glyph) const
{
    if (!glyph) return nullptr;

    FontBucketKey key{ glyph->mode, glyph->bucketID };
    auto atlasBucket = FindBucket(key);
    Assert(atlasBucket);

    return atlasBucket->GetMaterial(glyph->pageIndex);
}

std::shared_ptr<MaterialResource> FontAtlas::GetSolidMaterial() const
{
    return const_cast<AtlasPage&>(m_solidPage).GetMaterialResource();
}

const Resource& FontAtlas::GetAtlasResource(const FontBucketKey& key, uint16_t pageIndex) const
{
    auto atlasBucket = FindBucket(key);
    Assert(atlasBucket);

    return atlasBucket->GetAtlasResource(pageIndex);
}

FontAtlasBucket* FontAtlas::GetOrCreateBucket(const FontBucketKey& key)
{
    auto iter = m_buckets.find(key);
    if (iter != m_buckets.end())
        return iter->second.get();

    Size atlasSize{};
    std::unique_ptr<FontAtlasBucket> fontAtlasBucket{ nullptr };
    switch (key.mode)
    {
    case TextRenderMode::Bitmap:
        fontAtlasBucket = std::make_unique<BitmapFontAtlasBucket>(m_device, m_factory, key.bucket);
        atlasSize = m_textConfig.bitmap.atlasSize;
        break;
    case TextRenderMode::MTSDF:
        fontAtlasBucket = std::make_unique<MTSDFFontAtlasBucket>(m_device, m_factory, key.bucket);
        atlasSize = m_textConfig.mtsdf.atlasSize;
        break;
    }
    fontAtlasBucket->Initialize(atlasSize);

    auto result = fontAtlasBucket.get();
    m_buckets.emplace(key, std::move(fontAtlasBucket));

    return result;
}

FontAtlasBucket* FontAtlas::FindBucket(const FontBucketKey& key) const
{
    auto iter = m_buckets.find(key);
    if (iter == m_buckets.end())
        return nullptr;

    return iter->second.get();
}

