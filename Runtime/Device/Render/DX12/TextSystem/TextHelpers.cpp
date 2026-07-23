#include "pch.h"
#include "TextHelpers.h"
#include "Core/Foundation/Color.h"
#include "Atlas/Glyph/GlyphTypes.h"
#include <ft2build.h>
#include FT_FREETYPE_H

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

GlyphInfo CreateEmptyGlyphInfo(const SDFGlyph& glyph)
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
    uint32_t packX,
    uint32_t packY,
    uint32_t padding,
    const Size& atlasSize,
    GlyphInfo& info)
{
    float atlasW = static_cast<float>(atlasSize.width);
    float atlasH = static_cast<float>(atlasSize.height);

    float glyphX = static_cast<float>(packX + padding);
    float glyphY = static_cast<float>(packY + padding);

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
    uint32_t packX,
    uint32_t packY,
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
        packX, packY, 
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
    const SDFGlyph& glyph,
    FontBucketID bucketID,
    uint16_t pageIndex,
    uint32_t packX,
    uint32_t packY,
    uint32_t padding,
    const Size& atlasSize,
    float scale)
{
    GlyphInfo info;

    info.mode = TextRenderMode::SDF;
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
        packX, packY, 
        padding, atlasSize, info);
    return info;
}

GlyphInfo CreateGlyphInfo(
    const MTSDFGlyph& glyph,
    FontBucketID bucketID,
    uint16_t pageIndex,
    uint32_t packX,
    uint32_t packY,
    uint32_t padding,
    const Size& atlasSize,
    float scale)
{
    GlyphInfo info;

    info.mode = TextRenderMode::MTSDF;
    info.bucketID = bucketID;
    info.pageIndex = pageIndex;

    auto& pixels = glyph.pixels;
    auto scaledMetrics = ScaleMetrics(glyph.metrics, scale);

    //info.width = static_cast<float>(pixels.width); // sdf는 거리장을 더한 만큼 크게 그린다. 즉 글자 사각형이 겹쳐지게 그려진다.
    //info.height = static_cast<float>(pixels.height);
    info.width = scaledMetrics.width;
    info.height = scaledMetrics.height;
    info.bearingX = scaledMetrics.bearingX;
    info.bearingY = scaledMetrics.bearingY;

    SetGlyphUV(
        static_cast<float>(pixels.width),
        static_cast<float>(pixels.height),
        packX, packY,
        padding, atlasSize, info);
    return info;
}

bool CreateUploadEntry(
    GlyphPixels pixels,
    FontBucketID bucketID,
    uint16_t pageIndex,
    uint32_t packX,
    uint32_t packY,
    uint32_t padding,
    GlyphUploadEntry& outEntry)
{
    if (pixels.width == 0 || pixels.height == 0)
        return false;

    outEntry.x = packX + padding;
    outEntry.y = packY + padding;
    outEntry.pixels = std::move(pixels);

    return true;
}

static UIRenderMode ToUIRenderMode(TextRenderMode mode)
{
    switch (mode)
    {
    case TextRenderMode::SDF: return UIRenderMode::SDF;
    case TextRenderMode::MTSDF: return UIRenderMode::MTSDF;
    case TextRenderMode::Bitmap: return UIRenderMode::BitmapText;
    }
    Assert(false);

    return UIRenderMode::SDF;
}

void AppendGlyphQuad(
    std::vector<UIVertex>& vertices,
    std::vector<uint32_t>& indices,
    uint32_t& vertexOffset,
    const GlyphInfo& glyph,
    float x,
    float y,
    const Core::Color& color,
    UINT textureIndex)
{
    float x1 = x + glyph.width;
    float y1 = y + glyph.height;
    auto mode = ToUIRenderMode(glyph.mode);
    const float pxRange = glyph.pxRange;

    vertices.push_back(
        {
            { x, y, 0.f },
            color,
            { glyph.uvMin.x, glyph.uvMin.y },
            mode,
            textureIndex,
            pxRange
        });

    vertices.push_back(
        {
            { x1, y, 0.f },
            color,
            { glyph.uvMax.x, glyph.uvMin.y },
            mode,
            textureIndex,
            pxRange
        });

    vertices.push_back(
        {
            { x1, y1, 0.f },
            color,
            { glyph.uvMax.x, glyph.uvMax.y },
            mode,
            textureIndex,
            pxRange
        });

    vertices.push_back(
        {
            { x, y1, 0.f },
            color,
            { glyph.uvMin.x, glyph.uvMax.y },
            mode,
            textureIndex,
            pxRange
        });

    indices.insert(indices.end(), {
        vertexOffset + 0, vertexOffset + 1, vertexOffset + 2,
        vertexOffset + 0, vertexOffset + 2, vertexOffset + 3
        });

    vertexOffset += 4;
}