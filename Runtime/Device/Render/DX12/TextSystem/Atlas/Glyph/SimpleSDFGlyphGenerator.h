#pragma once
#include "GlyphGenerator.h"

struct FT_Bitmap_;
using FT_Bitmap = FT_Bitmap_;

class SimpleSDFGlyphGenerator : public GlyphGenerator
{
public:
    virtual GlyphBitmap Generate(
        FontResource* font,
        uint32_t glyphIndex,
        uint32_t size) override;

private:
    static bool IsInside(
        const FT_Bitmap& bitmap,
        int x,
        int y);

    static float DistanceToEdge(
        const FT_Bitmap& bitmap,
        int px,
        int py);
};