#pragma once
#include "Core/Foundation/Geometry2D.h"
#include "AtlasPage.h"
#include "GlyphCache.h"

struct GlyphUploadEntry;
struct ShapedText;
struct GlyphInfo;
struct IBrushResource;
class FontResource;
class Resource;
class Device;
class DescriptorFactory;

class FontAtlasBucket
{
public:
    virtual ~FontAtlasBucket() = default;
    FontAtlasBucket() = delete; 
    FontAtlasBucket(
        Device& device,
        DescriptorFactory& factory,
        FontBucketID bucketID);

    virtual void EnsureGlyphs(
        const ShapedText& shapedText,
        std::vector<std::vector<GlyphUploadEntry>>& outUploadsPerPage) = 0;

    void Initialize(const Size& atlasTextureSize);
    const GlyphInfo* FindGlyph(
        FontResource* font,
        uint32_t glyphIndex,
        uint32_t size) const;

    std::shared_ptr<IBrushResource> GetBrush(uint16_t pageIndex) const;
    const Resource& GetAtlasResource(uint16_t pageIndex) const;

protected:
    void CreatePage();
    uint16_t CurrentPageIndex() const;
    virtual GlyphPixelFormat GetAtlasPageFormat() const = 0;

    FontBucketID m_bucketID{ InvalidFontBucket };
    Size m_atlasTextureSize{};
    GlyphCache m_glyphCache;
    std::vector<std::unique_ptr<AtlasPage>> m_pages;

private:
    Device& m_device;
    DescriptorFactory& m_factory;
};