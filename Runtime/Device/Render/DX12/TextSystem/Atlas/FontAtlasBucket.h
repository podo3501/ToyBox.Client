#pragma once
#include "Core/Foundation/Geometry2D.h"

struct GlyphUploadEntry;
struct ShapedText;
struct GlyphInfo;
struct IMaterialResource;
class FontResource;
class Resource;

struct FontAtlasBucket
{
    virtual ~FontAtlasBucket() = default;
    virtual void Initialize(const Size& atlasTextureSize) = 0;
    virtual void EnsureGlyphs(
        const ShapedText& shapedText,
        std::vector<std::vector<GlyphUploadEntry>>& outUploadsPerPage) = 0;
    virtual const GlyphInfo* FindGlyph(
        FontResource* font,
        uint32_t glyphIndex,
        uint32_t size) const = 0;

    virtual std::shared_ptr<IMaterialResource> GetMaterial(uint16_t pageIndex) const = 0;
    virtual const Resource& GetAtlasResource(uint16_t pageIndex) const = 0;
};