#pragma once
#include "GlyphTypes.h"

class FontResource;

BitmapGlyph GenerateBitmapGlyph(FontResource* font, uint32_t glyphIndex, uint32_t size);