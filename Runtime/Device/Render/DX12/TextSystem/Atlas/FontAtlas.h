#pragma once
#include "FontAtlasBucket.h"

class Device;
class DescriptorFactory;
class FontResource;

struct FontAtlasUpload
{
    std::vector<std::vector<GlyphUploadEntry>> pages;
};

class FontAtlas
{
public:
    ~FontAtlas();
    FontAtlas(Device& device, DescriptorFactory& factory);
    bool Initialize(const Size& atlasTextureSize);
    void EnsureGlyphs(
        const ShapedText& shapedText,
        std::unordered_map<FontBucketID, FontAtlasUpload>& uploads);
    const GlyphInfo* FindGlyph(
        FontResource* font,
        uint32_t glyphIndex,
        uint32_t size) const;

    std::shared_ptr<IMaterialResource> GetMaterial(
        FontBucketID bucket,
        uint16_t pageIndex) const;
    const Resource& GetAtlasResource(
        FontBucketID bucket,
        uint16_t pageIndex) const;

private:
    FontAtlasBucket& GetOrCreateBucket(FontBucketID bucket);
    const FontAtlasBucket* FindBucket(FontBucketID bucket) const;

    Device& m_device;
    DescriptorFactory& m_factory;

    Size m_atlasTextureSize{};
    std::unordered_map<FontBucketID, FontAtlasBucket> m_buckets;
};