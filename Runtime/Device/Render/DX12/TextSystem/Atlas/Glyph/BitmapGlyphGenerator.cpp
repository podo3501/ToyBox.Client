#include "pch.h"
#include "BitmapGlyphGenerator.h"
#include "Resource/Font/FontResource.h"

BitmapGlyph BitmapGlyphGenerator::Generate(
    FontResource* font,
    uint32_t glyphIndex,
    uint32_t size)
{
    Assert(font);

    FT_GlyphSlot slot = font->GetGlyphSlot(glyphIndex, size);
    Assert(slot);

    BitmapGlyph bitmap;
    auto& pixels = bitmap.pixels;
    auto& metrics = bitmap.metrics;

    pixels.format = GlyphPixelFormat::R8;
    pixels.width = slot->bitmap.width;
    pixels.height = slot->bitmap.rows;

    metrics.width = static_cast<float>(slot->bitmap.width);
    metrics.height = static_cast<float>(slot->bitmap.rows);
    metrics.bearingX = static_cast<float>(slot->bitmap_left);
    metrics.bearingY = static_cast<float>(slot->bitmap_top);
    metrics.advanceX = static_cast<float>(slot->advance.x >> 6);

    if (pixels.width == 0 || pixels.height == 0)
        return bitmap;

    size_t pixelCount = pixels.width * pixels.height * GetBytesPerPixel(pixels.format);
    pixels.buffer.resize(pixelCount);

    for (uint32_t y = 0; y < pixels.height; ++y)
    {
        memcpy(
            pixels.buffer.data() + y * pixels.width,
            slot->bitmap.buffer + y * slot->bitmap.pitch,
            pixels.width);
    }

    return bitmap;
}