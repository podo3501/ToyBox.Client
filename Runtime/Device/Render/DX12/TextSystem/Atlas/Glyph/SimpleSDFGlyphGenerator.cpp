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

SDFGlyph SimpleSDFGlyphGenerator::Generate(
    FontResource* font,
    uint32_t glyphIndex,
    uint32_t size)
{
    Assert(font);

    FT_GlyphSlot slot = font->GetGlyphSlot(glyphIndex, size);
    Assert(slot);

    SDFGlyph sdfGlyph;
    auto& pixels = sdfGlyph.pixels;
    auto& metrics = sdfGlyph.metrics;

    pixels.format = GlyphPixelFormat::R8;
    pixels.width = slot->bitmap.width + SDFPadding * 2;
    pixels.height = slot->bitmap.rows + SDFPadding * 2;

    metrics.width = static_cast<float>(slot->bitmap.width);
    metrics.height = static_cast<float>(slot->bitmap.rows);
    metrics.bearingX = static_cast<float>(slot->bitmap_left - SDFPadding);
    metrics.bearingY = static_cast<float>(slot->bitmap_top + SDFPadding);
    metrics.advanceX = static_cast<float>(slot->advance.x >> 6);

    if (metrics.width == 0.f || metrics.height == 0.f)
        return sdfGlyph;

    size_t pixelCount = pixels.width * pixels.height * GetBytesPerPixel(pixels.format);
    pixels.buffer.resize(pixelCount);

    float minValue = FLT_MAX;
    float maxValue = -FLT_MAX;

    for (uint32_t y = 0; y < pixels.height; ++y)
    {
        for (uint32_t x = 0; x < pixels.width; ++x)
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

            pixels.buffer[y * pixels.width + x] =
                static_cast<uint8_t>(v * 255.0f);

            minValue = std::min(minValue, dist);
            maxValue = std::max(maxValue, dist);
        }
    }

    return sdfGlyph;
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