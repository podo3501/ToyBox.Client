#include "pch.h"
#include "FontAtlas.h"
#include "Glyph/BitmapGlyphGenerator.h"
#include "Glyph/SDFGlyphGenerator.h"

FontAtlas::~FontAtlas() = default;
FontAtlas::FontAtlas(Device& device, DescriptorFactory& factory) :
    m_device{ device },
    m_factory{ factory }
{}

bool FontAtlas::Initialize(const Size& atlasTextureSize)
{
    if (atlasTextureSize.width <= 0 || atlasTextureSize.height <= 0)
        return false;

    m_atlasTextureSize = atlasTextureSize;
    return true;
}

void FontAtlas::EnsureGlyphs(
    const ShapedText& shapedText,
    std::unordered_map<FontBucketID, FontAtlasUpload>& uploads)
{
    FontBucketID bucket = GetFontBucketID(shapedText.size);
    auto& upload = uploads[bucket];

    auto& atlasBucket = GetOrCreateBucket(bucket);
    atlasBucket.EnsureGlyphs(
        shapedText,
        upload.pages);
}

const GlyphInfo* FontAtlas::FindGlyph(
    FontResource* font,
    uint32_t glyphIndex,
    uint32_t size) const
{
    FontBucketID bucket = GetFontBucketID(size);
    auto atlasBucket = FindBucket(bucket);
    if (!atlasBucket)
        return nullptr;
    
    return atlasBucket->FindGlyph(
        font,
        glyphIndex,
        size);
}

std::shared_ptr<IMaterialResource> FontAtlas::GetMaterial(
    FontBucketID bucket,
    uint16_t pageIndex) const
{
    auto atlasBucket = FindBucket(bucket);
    Assert(atlasBucket);

    return atlasBucket->GetMaterial(
        pageIndex);
}

const Resource& FontAtlas::GetAtlasResource(
    FontBucketID bucket,
    uint16_t pageIndex) const
{    
    auto atlasBucket = FindBucket(bucket);
    Assert(atlasBucket);

    return atlasBucket->GetAtlasResource(pageIndex);
}

FontAtlasBucket& FontAtlas::GetOrCreateBucket(FontBucketID bucket)
{
    Assert(
        bucket == FontBuckets::Small ||
        bucket == FontBuckets::Medium ||
        bucket == FontBuckets::Large ||
        bucket == FontBuckets::Huge);

    //auto glyphGenerator = std::make_unique<BitmapGlyphGenerator>();
    auto glyphGenerator = std::make_unique<SDFGlyphGenerator>();
    auto [iter, inserted] = m_buckets.try_emplace(
        bucket,
        m_device,
        m_factory,
        std::move(glyphGenerator),
        bucket);

    if (inserted)
        iter->second.Initialize(m_atlasTextureSize);

    return iter->second;
}

const FontAtlasBucket* FontAtlas::FindBucket(FontBucketID bucket) const
{
    auto iter = m_buckets.find(bucket);
    if (iter == m_buckets.end())
        return nullptr;

    return &iter->second;
}