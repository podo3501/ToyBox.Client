#pragma once
#include "FontAtlasBucket.h"
#include "AtlasPage.h"
#include "GlyphCache.h"

class Device;
class DescriptorFactory;
class FontResource;
class GlyphGenerator;

class BitmapFontAtlasBucket : public FontAtlasBucket
{
public:
    virtual ~BitmapFontAtlasBucket() override;
    BitmapFontAtlasBucket(
        Device& device,
        DescriptorFactory& factory,
        FontBucketID bucketID);

    virtual void Initialize(const Size& atlasTextureSize) override;
    virtual void EnsureGlyphs(
        const ShapedText& shapedText,
        std::vector<std::vector<GlyphUploadEntry>>& outUploadsPerPage) override;
    virtual const GlyphInfo* FindGlyph(
        FontResource* font,
        uint32_t glyphIndex,
        uint32_t size) const override;

    virtual std::shared_ptr<IMaterialResource> GetMaterial(uint16_t pageIndex) const override;
    virtual const Resource& GetAtlasResource(uint16_t pageIndex) const override;

private:
    void CreatePage();
    AtlasPageDesc GetAtlasPageDesc() const;
    uint16_t CurrentPageIndex() const;

    Device& m_device;
    DescriptorFactory& m_factory;

    FontBucketID m_bucketID{ InvalidFontBucket };
    Size m_atlasTextureSize{};
    GlyphCache m_glyphCache;
    std::vector<std::unique_ptr<AtlasPage>> m_pages;
};