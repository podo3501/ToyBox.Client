#include "pch.h"
#include "TextHelpers.h"
#include "Core/Foundation/Color.h"
#include "Atlas/Glyph/GlyphBitmap.h"
#include <ft2build.h>
#include FT_FREETYPE_H

GlyphInfo CreateEmptyGlyphInfo(const GlyphBitmap& glyph)
{
    GlyphInfo info;

    info.width = glyph.glyphWidth;
    info.height = glyph.glyphHeight;
    info.bearingX = glyph.bearingX;
    info.bearingY = glyph.bearingY;

    return info;
}

GlyphInfo CreateGlyphInfo(
    const GlyphBitmap& glyph,
    FontBucketID bucketID,
    uint16_t pageIndex,
    uint32_t packX,
    uint32_t packY,
    uint32_t padding,
    const Size& atlasSize)
{
    GlyphInfo info;

    info.bucketID = bucketID;
    info.pageIndex = pageIndex;

    info.width = glyph.glyphWidth;
    info.height = glyph.glyphHeight;
    info.bearingX = glyph.bearingX;
    info.bearingY = glyph.bearingY;

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
        (glyphX + info.width) / atlasW,
        (glyphY + info.height) / atlasH
    };

    return info;
}

bool CreateUploadEntry(
    GlyphBitmap bitmap,
    FontBucketID bucketID,
    uint16_t pageIndex,
    uint32_t packX,
    uint32_t packY,
    uint32_t padding,
    GlyphUploadEntry& outEntry)
{
    if (bitmap.width == 0 || bitmap.height == 0)
        return false;

    outEntry.bucketID = bucketID;
    outEntry.pageIndex = pageIndex;
    outEntry.x = packX + padding;
    outEntry.y = packY + padding;
    outEntry.bitmap = std::move(bitmap);

    return true;
}

void AppendGlyphQuad(
    std::vector<UIVertex>& vertices,
    std::vector<uint32_t>& indices,
    uint32_t& vertexOffset,
    const GlyphInfo& glyph,
    float x,
    float y,
    const Core::Color& color)
{
    float x1 = x + glyph.width;
    float y1 = y + glyph.height;

    vertices.push_back(
        {
            { x, y, 0.f },
            color,
            { glyph.uvMin.x, glyph.uvMin.y }
        });

    vertices.push_back(
        {
            { x1, y, 0.f },
            color,
            { glyph.uvMax.x, glyph.uvMin.y }
        });

    vertices.push_back(
        {
            { x1, y1, 0.f },
            color,
            { glyph.uvMax.x, glyph.uvMax.y }
        });

    vertices.push_back(
        {
            { x, y1, 0.f },
            color,
            { glyph.uvMin.x, glyph.uvMax.y }
        });

    indices.insert(indices.end(), {
        vertexOffset + 0, vertexOffset + 1, vertexOffset + 2,
        vertexOffset + 0, vertexOffset + 2, vertexOffset + 3
        });

    vertexOffset += 4;
}