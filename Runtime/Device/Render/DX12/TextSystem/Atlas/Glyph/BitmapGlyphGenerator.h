#pragma once
#include "GlyphTypes.h"

class FontResource;

class BitmapGlyphGenerator
{
public:
    BitmapGlyph Generate(FontResource* font, uint32_t glyphIndex, uint32_t size);
};