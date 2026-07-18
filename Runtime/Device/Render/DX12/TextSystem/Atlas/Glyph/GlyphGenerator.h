#pragma once
#include <cstdint>
#include "GlyphBitmap.h"

class FontResource;

class GlyphGenerator
{
public:
    virtual ~GlyphGenerator() = default;
    virtual GlyphBitmap Generate(FontResource* font, uint32_t glyphIndex, uint32_t size) = 0;
};