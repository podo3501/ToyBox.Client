#include "pch.h"
#include "SimpleSDFGlyphGenerator.h"
#include "Resource/Font/FontResource.h"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace
{
    constexpr int Padding = 8;
    constexpr float SDFRange = 8.0f;
}

GlyphBitmap SimpleSDFGlyphGenerator::Generate(
    FontResource* font,
    uint32_t glyphIndex,
    uint32_t size)
{
    Assert(font);

    FT_GlyphSlot slot = font->GetGlyphSlot(glyphIndex, size);
    Assert(slot);

    GlyphBitmap bitmap;

    bitmap.bearingX = static_cast<float>(slot->bitmap_left - Padding);
    bitmap.bearingY = static_cast<float>(slot->bitmap_top + Padding);
    bitmap.advanceX = static_cast<float>(slot->advance.x >> 6);

    if (slot->bitmap.width == 0 ||
        slot->bitmap.rows == 0)
    {
        return bitmap;
    }

    bitmap.width = slot->bitmap.width + Padding * 2;
    bitmap.height = slot->bitmap.rows + Padding * 2;
    bitmap.channels = 1;

    bitmap.glyphWidth = static_cast<float>(slot->bitmap.width);
    bitmap.glyphHeight = static_cast<float>(slot->bitmap.rows);

    bitmap.pixels.resize(bitmap.width * bitmap.height);

    float minValue = FLT_MAX;
    float maxValue = -FLT_MAX;

    for (uint32_t y = 0; y < bitmap.height; ++y)
    {
        for (uint32_t x = 0; x < bitmap.width; ++x)
        {
            int srcX = static_cast<int>(x) - Padding;
            int srcY = static_cast<int>(y) - Padding;

            bool inside = false;

            if (srcX >= 0 &&
                srcX < static_cast<int>(slot->bitmap.width) &&
                srcY >= 0 &&
                srcY < static_cast<int>(slot->bitmap.rows))
            {
                inside = IsInside(slot->bitmap, srcX, srcY);
            }

            float dist = DistanceToEdge(
                slot->bitmap,
                srcX,
                srcY);

            if (!inside)
                dist = -dist;

            float v = std::clamp(
                dist / SDFRange + 0.5f,
                0.0f,
                1.0f);

            bitmap.pixels[y * bitmap.width + x] =
                static_cast<uint8_t>(v * 255.0f);

            minValue = std::min(minValue, dist);
            maxValue = std::max(maxValue, dist);
        }
    }

    return bitmap;
}

bool SimpleSDFGlyphGenerator::IsInside(
    const FT_Bitmap& bitmap,
    int x,
    int y)
{
    return bitmap.buffer[y * bitmap.pitch + x] > 127;
}

float SimpleSDFGlyphGenerator::DistanceToEdge(
    const FT_Bitmap& bitmap,
    int px,
    int py)
{
    const int width = static_cast<int>(bitmap.width);
    const int height = static_cast<int>(bitmap.rows);

    bool inside = false;

    if (px >= 0 &&
        px < width &&
        py >= 0 &&
        py < height)
    {
        inside = IsInside(bitmap, px, py);
    }

    float minDist2 = FLT_MAX;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            if (IsInside(bitmap, x, y) == inside)
                continue;

            float dx = static_cast<float>(x - px);
            float dy = static_cast<float>(y - py);

            float d2 = dx * dx + dy * dy;

            if (d2 < minDist2)
                minDist2 = d2;
        }
    }

    return std::sqrt(minDist2);
}