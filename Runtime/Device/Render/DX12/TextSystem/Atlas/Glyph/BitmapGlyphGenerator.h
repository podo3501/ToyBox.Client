#pragma once
#include "GlyphBitmap.h"

class FontResource;

class BitmapGlyphGenerator
{
public:
    GlyphBitmap Generate(FontResource* font, uint32_t glyphIndex, uint32_t size);
};