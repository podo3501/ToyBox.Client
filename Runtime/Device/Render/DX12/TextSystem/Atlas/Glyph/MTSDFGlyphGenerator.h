#pragma once
#include "GlyphTypes.h"

class FontResource;

MTSDFGlyph GenerateMTSDFGlyph(FontResource* font, uint32_t glyphIndex, uint32_t size);
