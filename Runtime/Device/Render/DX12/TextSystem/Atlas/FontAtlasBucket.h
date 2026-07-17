#pragma once
#include "AtlasPage.h"
#include "GlyphCache.h"

class Device;
class DescriptorFactory;
class FontResource;

class FontAtlasBucket
{
public:
    ~FontAtlasBucket();
    FontAtlasBucket(
        Device& device,
        DescriptorFactory& factory,
        FontBucketID bucketID);

    void Initialize(const Size& atlasTextureSize);
    void EnsureGlyphs(
        const ShapedText& shapedText,
        std::vector<std::vector<GlyphUploadEntry>>& outUploadsPerPage);
    const GlyphInfo* FindGlyph(
        FontResource* font,
        uint32_t glyphIndex,
        uint32_t size) const;

    std::shared_ptr<IMaterialResource> GetMaterial(uint16_t pageIndex) const;
    const Resource& GetAtlasResource(uint16_t pageIndex) const;
    uint16_t PageCount() const;

private:
    void CreatePage();
    uint16_t CurrentPageIndex() const;

    Device& m_device;
    DescriptorFactory& m_factory;
    FontBucketID m_bucketID{ InvalidFontBucket };

    Size m_atlasTextureSize{};
    GlyphCache m_glyphCache;
    std::vector<std::unique_ptr<AtlasPage>> m_pages;
};