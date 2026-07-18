#pragma once
#include "GlyphGenerator.h"

class BitmapGlyphGenerator final : public GlyphGenerator
{
public:
    virtual GlyphBitmap Generate(FontResource* font, uint32_t glyphIndex, uint32_t size) override;
};