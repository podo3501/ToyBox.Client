#pragma once
#include "TextTypes.h"

void ApplyWordWrap(std::span<ShapedGlyph> glyphs, float maxWidth, bool wordWrap);
