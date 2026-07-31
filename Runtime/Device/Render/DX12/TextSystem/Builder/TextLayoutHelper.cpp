#include "pch.h"
#include "TextLayoutHelper.h"
#include "Core/RenderData.h"

float ComputeHorizontalOffset(
    TextHorizontalAlign align,
    float boundsWidth,
    float lineWidth)
{
    if (boundsWidth <= 0.f)
        return 0.f; // bounds width가 없으면 정렬 기준이 없으므로 Left와 동일

    switch (align)
    {
    case TextHorizontalAlign::Center: return (boundsWidth - lineWidth) * 0.5f;
    case TextHorizontalAlign::Right:  return (boundsWidth - lineWidth);
    case TextHorizontalAlign::Left:
    default: return 0.f;
    }
}

float ComputeVerticalOffset(
    TextVerticalAlign align,
    float boundsHeight,
    float totalBlockHeight)
{
    if (boundsHeight <= 0.f)
        return 0.f; // bounds height가 없으면 정렬 기준이 없으므로 Top과 동일

    switch (align)
    {
    case TextVerticalAlign::Middle: return (boundsHeight - totalBlockHeight) * 0.5f;
    case TextVerticalAlign::Bottom: return (boundsHeight - totalBlockHeight);
    case TextVerticalAlign::Top:
    default: return 0.f;
    }
}

std::vector<float> ComputeLineWidths(std::span<const ShapedGlyph> glyphs)
{
    std::vector<float> lineWidths;
    for (const auto& g : glyphs)
    {
        if (g.lineIndex >= lineWidths.size())
            lineWidths.resize(g.lineIndex + 1, 0.f);
        lineWidths[g.lineIndex] += g.advanceX;
    }
    return lineWidths;
}

Rect ComputeClipRect(const DrawTextItem& item)
{
    if (item.layout.overflow != TextOverflow::Clip)
        return {};

    constexpr float kUnbounded = std::numeric_limits<float>::max(); // clip이 꺼져있거나 해당 축 size가 지정 안 되어 있으면 그 축은 무제한으로 열어둔다.
    return Rect{
        item.position.x,
        item.position.y,
        item.size.x > 0.f ? item.size.x : kUnbounded,
        item.size.y > 0.f ? item.size.y : kUnbounded
    };
}