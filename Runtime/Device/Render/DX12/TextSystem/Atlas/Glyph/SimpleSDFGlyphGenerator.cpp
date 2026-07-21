#include "pch.h"
#include "SimpleSDFGlyphGenerator.h"
#include "Resource/Font/FontResource.h"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace
{
    constexpr int SDFPadding = 8; //거리장을 위한 padding
    constexpr float SDFRange = 8.0f; //수치 범위 -에서 + 까지.
}

SDFGlyphBitmap SimpleSDFGlyphGenerator::Generate(
    FontResource* font,
    uint32_t glyphIndex,
    uint32_t size)
{
    Assert(font);

    FT_GlyphSlot slot = font->GetGlyphSlot(glyphIndex, size);
    Assert(slot);

    SDFGlyphBitmap sdfGlyphBitmap;
    auto& bitmap = sdfGlyphBitmap.bitmap;

    bitmap.format = GlyphPixelFormat::R8;
    bitmap.bearingX = static_cast<float>(slot->bitmap_left - SDFPadding);
    bitmap.bearingY = static_cast<float>(slot->bitmap_top + SDFPadding);
    bitmap.advanceX = static_cast<float>(slot->advance.x >> 6);

    if (slot->bitmap.width == 0 ||
        slot->bitmap.rows == 0)
    {
        return sdfGlyphBitmap;
    }

    bitmap.width = slot->bitmap.width + SDFPadding * 2;
    bitmap.height = slot->bitmap.rows + SDFPadding * 2;

    sdfGlyphBitmap.glyphWidth = static_cast<float>(slot->bitmap.width);
    sdfGlyphBitmap.glyphHeight = static_cast<float>(slot->bitmap.rows);

    size_t pixelCount = bitmap.width * bitmap.height * GetBytesPerPixel(bitmap.format);
    bitmap.pixels.resize(pixelCount);

    float minValue = FLT_MAX;
    float maxValue = -FLT_MAX;

    for (uint32_t y = 0; y < bitmap.height; ++y)
    {
        for (uint32_t x = 0; x < bitmap.width; ++x)
        {
            int srcX = static_cast<int>(x) - SDFPadding;
            int srcY = static_cast<int>(y) - SDFPadding;

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

    return sdfGlyphBitmap;
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