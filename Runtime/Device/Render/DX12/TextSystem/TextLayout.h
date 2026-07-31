#pragma once
#include "TextTypes.h"

struct DrawTextRun;

void ApplyWordWrap(
    std::span<ShapedGlyph> glyphs, 
    float maxWidth, 
    bool wordWrap);

std::vector<ShapedGlyph> ShapeRuns(
    FontResource* font,
    std::span<const DrawTextRun> runs,
    uint32_t fontSize);