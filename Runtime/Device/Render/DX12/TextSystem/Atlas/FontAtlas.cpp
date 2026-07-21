#include "pch.h"
#include "FontAtlas.h"
#include "Glyph/BitmapGlyphGenerator.h"
#include "Glyph/SDFGlyphGenerator.h"
#include "Glyph/SimpleSDFGlyphGenerator.h"
#include "../Builder/FontAtlasUploadGraphBuilder.h"
#include "BitmapFontAtlasBucket.h"
#include "SDFFontAtlasBucket.h"

static FontBucketID GetFontBucketID(TextRenderMode mode, uint32_t size)
{
    switch (mode)
    {
    case TextRenderMode::Bitmap:
        if (size <= BitmapBuckets::Small) return BitmapBuckets::Small;
        if (size <= BitmapBuckets::Medium) return BitmapBuckets::Medium;
        if (size <= BitmapBuckets::Large) return BitmapBuckets::Large;
        break;

    case TextRenderMode::SDF:
        if (size <= SDFBuckets::Small) return SDFBuckets::Small;
        if (size <= SDFBuckets::Medium) return SDFBuckets::Medium;
        if (size <= SDFBuckets::Large) return SDFBuckets::Large;
        if (size <= SDFBuckets::Huge) return SDFBuckets::Huge;
        break;
    }
    Assert(false);

    return InvalidFontBucket;
}

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
    return true;
}

void FontAtlas::EnsureGlyphs(TextRenderMode mode, std::span<const ShapedText> shapedTexts)
{
    std::unordered_map<FontBucketKey, FontAtlasUpload, FontBucketKeyHash> uploads;

    for (const auto& shapedText : shapedTexts)
    {
        FontBucketID bucket = GetFontBucketID(mode, shapedText.size);

        FontBucketKey key{ mode, bucket };
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

    std::unique_ptr<FontAtlasBucket> fontAtlasBucket{ nullptr };
    switch (key.mode)
    {
    case TextRenderMode::Bitmap:
        fontAtlasBucket = std::make_unique<BitmapFontAtlasBucket>(
            m_device,
            m_factory,
            key.bucket);
        break;
    case TextRenderMode::SDF:
        fontAtlasBucket = std::make_unique<SDFFontAtlasBucket>(
            m_device,
            m_factory,
            key.bucket);
        break;
    }
    
    fontAtlasBucket->Initialize(m_textConfig.bitmap.atlasSize);

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

//FontAtlasBucket을 상속받은 bitmapAtlasBucket, sdfAtlasBucket으로 한다.
//현재 AtlasBucket에서 같이 쓰고 있는 구조체를 bitmap과 sdf용으로 나눈다.
//BitmapGlyphGenerator는 AtlasBucket에서 생성하게 한다.
//bitmap과 sdf의 texture 크기를 달리 해서 테스트 해 본다.
//bitmap, sdf 가 동시에 찍히는지 확인 한다.
//sdf 찍을때 오류 수정.
//sdf용 shader 만들기.
//일단 분리 어느정도 시키고 돌아가는게 확인되면 sdf 돌아가게끔 하기.
//sdf 돌아가게끔 하다가 분리가 필요하면 일단 멈추고 분리 시킨후 테스트 후 분리가 잘된거 확인후 sdf 진행.
