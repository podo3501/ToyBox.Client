#pragma once
#include "GlyphGenerator.h"

struct FT_GlyphSlotRec_;
using FT_GlyphSlot = FT_GlyphSlotRec_*;

namespace msdfgen
{
    class Shape;
    class Projection;
}

class SDFGlyphGenerator : public GlyphGenerator
{
public:
    virtual GlyphBitmap Generate(FontResource* font, uint32_t glyphIndex, uint32_t size) override;

private:
    msdfgen::Shape LoadShape(FT_GlyphSlot slot) const;
    msdfgen::Projection CreateProjection(
        const msdfgen::Shape& shape,
        uint32_t width,
        uint32_t height) const;
};