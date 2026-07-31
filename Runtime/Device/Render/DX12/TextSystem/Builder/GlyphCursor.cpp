#include "pch.h"
#include "GlyphCursor.h"
#include "../TextTypes.h"
#include "Core/RenderData.h"
#include "TextLayoutHelper.h"
#include "Resource/Font/FontResource.h"

GlyphCursor::GlyphCursor(
    const ShapedText& shaped,
    const DrawTextItem& item,
    std::span<const float> lineWidths) :
    m_item{ item },
    m_lineWidths{ lineWidths }
{
    float baseLineHeight = shaped.font->GetLineHeight(shaped.size);
    m_lineHeight = baseLineHeight * item.layout.lineSpacing;
    m_ascent = shaped.font->GetAscent(shaped.size);

    float totalBlockHeight = static_cast<float>(lineWidths.size()) * m_lineHeight;
    m_verticalOffset = ComputeVerticalOffset(item.layout.verticalAlign, item.size.y, totalBlockHeight);
    m_baseY = item.position.y + m_verticalOffset;

    m_baselineY = m_baseY + m_ascent;
    m_cursorX = item.position.x;
}

void GlyphCursor::BeginLine(uint32_t lineIndex)
{
    float lineWidth = (lineIndex < m_lineWidths.size()) ? m_lineWidths[lineIndex] : 0.f;
    float alignOffset = ComputeHorizontalOffset(m_item.layout.horizontalAlign, m_item.size.x, lineWidth);

    m_cursorX = m_item.position.x + alignOffset;
    m_baselineY = m_baseY + m_ascent + static_cast<float>(lineIndex) * m_lineHeight;
}

bool GlyphCursor::IsLineVisible(uint32_t lineIndex, const Rect& clipRect) const
{
    float lineTop = m_baseY + static_cast<float>(lineIndex) * m_lineHeight;
    float lineBottom = lineTop + m_lineHeight;
    return !(lineBottom < clipRect.Top() || lineTop > clipRect.Bottom());
}