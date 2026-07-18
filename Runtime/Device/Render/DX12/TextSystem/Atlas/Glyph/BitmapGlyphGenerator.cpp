#include "pch.h"
#include "BitmapGlyphGenerator.h"
#include "Resource/Font/FontResource.h"

GlyphBitmap BitmapGlyphGenerator::Generate(
    FontResource* font,
    uint32_t glyphIndex,
    uint32_t size)
{
    Assert(font);

    FT_GlyphSlot slot = font->GetGlyphSlot(glyphIndex, size);
    Assert(slot);

    GlyphBitmap bitmap;

    bitmap.width = slot->bitmap.width;
    bitmap.height = slot->bitmap.rows;

    bitmap.glyphWidth = static_cast<float>(slot->bitmap.width);
    bitmap.glyphHeight = static_cast<float>(slot->bitmap.rows);

    bitmap.bearingX = static_cast<float>(slot->bitmap_left);
    bitmap.bearingY = static_cast<float>(slot->bitmap_top);

    bitmap.channels = 1;
    bitmap.advanceX = static_cast<float>(slot->advance.x >> 6);

    if (bitmap.width == 0 || bitmap.height == 0)
        return bitmap;

    size_t pixelCount = bitmap.width * bitmap.height;
    bitmap.pixels.resize(pixelCount);

    for (uint32_t y = 0; y < bitmap.height; ++y)
    {
        memcpy(
            bitmap.pixels.data() + y * bitmap.width,
            slot->bitmap.buffer + y * slot->bitmap.pitch,
            bitmap.width);
    }

    return bitmap;
}