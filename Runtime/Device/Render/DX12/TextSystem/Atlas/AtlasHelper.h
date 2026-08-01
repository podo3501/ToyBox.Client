#pragma once
#include "../TextTypes.h"
#include "Core/Foundation/Geometry2D.h"

GlyphInfo CreateEmptyGlyphInfo(const BitmapGlyph& glyph);
GlyphInfo CreateEmptyGlyphInfo(const MTSDFGlyph& glyph);

GlyphInfo CreateGlyphInfo(
    const BitmapGlyph& glyph,
    FontBucketID bucketID,
    uint16_t pageIndex,
    const Point& packPos,
    uint32_t padding,
    const Size& atlasSize);

GlyphInfo CreateGlyphInfo(
    const MTSDFGlyph& glyph,
    FontBucketID bucketID,
    uint16_t pageIndex,
    const Point& packPos,
    uint32_t padding,
    const Size& atlasSize,
    float scale);

bool CreateUploadEntry(
    GlyphPixels&& pixels,
    const Point& packPos,
    uint32_t padding,
    GlyphUploadEntry& outEntry);