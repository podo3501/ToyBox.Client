#pragma once
#include "../TextTypes.h"
#include "Core/Foundation/Geometry2D.h"

struct TextBatchInfo;

struct PackedTextParams
{
    uint32_t params1;
    uint32_t params2;
};

std::vector<PackedTextParams> PackRunParams(const std::vector<TextRun>& runs);

void AppendGlyphQuad(
    TextBatchInfo& info,
    const GlyphInfo& glyph,
    float x, float y,
    const Core::Color& color,
    PackedTextParams packParams,
    const Rect& clipRect);

void AppendSolidQuad(
    TextBatchInfo& info,
    const Rect& rect,
    const Core::Color& color,
    const Rect& clipRect);
