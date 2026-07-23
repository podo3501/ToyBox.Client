#pragma once
#include "GlyphTypes.h"

struct FT_GlyphSlotRec_;
using FT_GlyphSlot = FT_GlyphSlotRec_*;

namespace msdfgen
{
    class Shape;
    class Projection;
}

class FontResource;

class SDFGlyphGenerator
{
public:
    SDFGlyph Generate(FontResource* font, uint32_t glyphIndex, uint32_t size);

//private:
//    msdfgen::Shape LoadShape(FT_GlyphSlot slot) const;
//    msdfgen::Projection CreateProjection(
//        const msdfgen::Shape& shape,
//        uint32_t width,
//        uint32_t height) const;
};