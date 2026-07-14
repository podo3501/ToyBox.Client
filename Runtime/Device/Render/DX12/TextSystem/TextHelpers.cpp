#include "pch.h"
#include "TextHelpers.h"
#include "Core/Foundation/Color.h"
#include <ft2build.h>
#include FT_FREETYPE_H

GlyphInfo CreateEmptyGlyphInfo(FT_GlyphSlot slot)
{
    GlyphInfo info;

    info.width = 0.f;
    info.height = 0.f;
    info.bearingX = static_cast<float>(slot->bitmap_left);
    info.bearingY = static_cast<float>(slot->bitmap_top);

    return info;
}

GlyphInfo CreateGlyphInfo(
    FT_GlyphSlot slot,
    uint32_t packX,
    uint32_t packY,
    uint32_t padding,
    const Size& atlasSize,
    uint16_t pageIndex)
{
    GlyphInfo info;

    uint32_t width = slot->bitmap.width;
    uint32_t height = slot->bitmap.rows;

    info.width = static_cast<float>(width);
    info.height = static_cast<float>(height);
    info.bearingX = static_cast<float>(slot->bitmap_left);
    info.bearingY = static_cast<float>(slot->bitmap_top);

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

    info.pageIndex = pageIndex;

    return info;
}

bool CreateUploadEntry(
    FT_GlyphSlot slot,
    uint32_t packX,
    uint32_t packY,
    uint32_t padding,
    GlyphUploadEntry& outEntry)
{
    uint32_t width = slot->bitmap.width;
    uint32_t height = slot->bitmap.rows;

    if (width == 0 || height == 0)
        return false;

    outEntry.width = width;
    outEntry.height = height;
    outEntry.packX = packX + padding;
    outEntry.packY = packY + padding;

    outEntry.pixelData.resize(width * height);

    for (uint32_t y = 0; y < height; ++y)
    {
        std::memcpy(
            &outEntry.pixelData[y * width],
            &slot->bitmap.buffer[y * slot->bitmap.pitch],
            width);
    }

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