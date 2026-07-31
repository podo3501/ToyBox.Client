#pragma once
#include "GameClient/Service/Render/Definition/Text/TextStyle.h"
#include "Core/Foundation/Geometry2D.h"
#include "../TextTypes.h"

struct DrawTextItem;

float ComputeHorizontalOffset(
    TextHorizontalAlign align,
    float boundsWidth,
    float lineWidth);

float ComputeVerticalOffset(
    TextVerticalAlign align,
    float boundsHeight,
    float totalBlockHeight);

std::vector<float> ComputeLineWidths(std::span<const ShapedGlyph> glyphs);
Rect ComputeClipRect(const DrawTextItem& item);
