#pragma once
#include "GlyphTypes.h"

class FontResource;

class MTSDFGlyphGenerator
{
public:
    MTSDFGlyph Generate(FontResource* font, uint32_t glyphIndex, uint32_t size);
};