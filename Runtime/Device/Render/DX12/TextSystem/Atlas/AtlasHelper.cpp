#include "pch.h"
#include "AtlasHelper.h"
#include "Glyph/GlyphTypes.h"

GlyphInfo CreateEmptyGlyphInfo(const BitmapGlyph& glyph)
{
    GlyphInfo info;

    auto& metrics = glyph.metrics;
    info.width = metrics.width;
    info.height = metrics.height;
    info.bearingX = metrics.bearingX;
    info.bearingY = metrics.bearingY;

    return info;
}

GlyphInfo CreateEmptyGlyphInfo(const MTSDFGlyph& glyph)
{
    GlyphInfo info;

    auto& metrics = glyph.metrics;
    info.width = metrics.width;
    info.height = metrics.height;
    info.bearingX = metrics.bearingX;
    info.bearingY = metrics.bearingY;

    return info;
}

static void SetGlyphUV(
    float width,
    float height,
    const Point& packPos,
    uint32_t padding,
    const Size& atlasSize,
    GlyphInfo& info)
{
    float atlasW = static_cast<float>(atlasSize.width);
    float atlasH = static_cast<float>(atlasSize.height);

    float glyphX = static_cast<float>(packPos.x + padding);
    float glyphY = static_cast<float>(packPos.y + padding);

    info.uvMin =
    {
        glyphX / atlasW,
        glyphY / atlasH
    };

    info.uvMax =
    {
        (glyphX + width) / atlasW,
        (glyphY + height) / atlasH
    };
}

GlyphInfo CreateGlyphInfo(
    const BitmapGlyph& glyph,
    FontBucketID bucketID,
    uint16_t pageIndex,
    const Point& packPos,
    uint32_t padding,
    const Size& atlasSize)
{
    GlyphInfo info;

    info.mode = TextRenderMode::Bitmap;
    info.bucketID = bucketID;
    info.pageIndex = pageIndex;

    auto& metrics = glyph.metrics;
    info.width = metrics.width;
    info.height = metrics.height;
    info.bearingX = metrics.bearingX;
    info.bearingY = metrics.bearingY;

    SetGlyphUV(
        static_cast<float>(glyph.pixels.width),
        static_cast<float>(glyph.pixels.height),
        packPos,
        padding, atlasSize, info);
    return info;
}

static GlyphMetrics ScaleMetrics(const GlyphMetrics& metrics, float scale)
{
    GlyphMetrics scaled;
    scaled.width = metrics.width * scale;
    scaled.height = metrics.height * scale;
    scaled.bearingX = metrics.bearingX * scale;
    scaled.bearingY = metrics.bearingY * scale;
    scaled.advanceX = metrics.advanceX * scale;
    return scaled;
}

GlyphInfo CreateGlyphInfo(
    const MTSDFGlyph& glyph,
    FontBucketID bucketID,
    uint16_t pageIndex,
    const Point& packPos,
    uint32_t padding,
    const Size& atlasSize,
    float scale)
{
    GlyphInfo info;

    info.mode = TextRenderMode::MTSDF;
    info.bucketID = bucketID;
    info.pageIndex = pageIndex;

    auto& pixels = glyph.pixels;
    info.pxRange = glyph.metrics.pxRange;

    auto scaledMetrics = ScaleMetrics(glyph.metrics, scale);
    info.width = scaledMetrics.width;
    info.height = scaledMetrics.height;
    info.bearingX = scaledMetrics.bearingX;
    info.bearingY = scaledMetrics.bearingY;

    SetGlyphUV(
        static_cast<float>(pixels.width),
        static_cast<float>(pixels.height),
        packPos,
        padding, atlasSize, info);
    return info;
}

bool CreateUploadEntry(
    GlyphPixels&& pixels,
    const Point& packPos,
    uint32_t padding,
    GlyphUploadEntry& outEntry)
{
    if (pixels.width == 0 || pixels.height == 0)
        return false;

    outEntry.x = packPos.x + padding;
    outEntry.y = packPos.y + padding;
    outEntry.pixels = std::move(pixels);

    return true;
}