#pragma once
#include "TextTypes.h"

void ApplyWordWrap(
    std::span<ShapedGlyph> glyphs, 
    float maxWidth, 
    bool wordWrap);

std::vector<ShapedGlyph> ShapeRuns(
    FontResource* font,
    std::span<const TextRun> runs,
    uint32_t fontSize);