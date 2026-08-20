#pragma once
#include "Core/Foundation/Geometry2D.h"

struct ShapedText;
struct RenderTextItem;

class GlyphCursor
{
public:
    GlyphCursor(
        const ShapedText& shaped,
        const RenderTextItem& item,
        std::span<const float> lineWidths);
    
    void BeginLine(uint32_t lineIndex); // 줄이 바뀔 때 호출 — cursorX/baselineY를 그 줄의 align에 맞게 리셋

    // 현재 상태 조회
    float CursorX() const { return m_cursorX; }
    float BaselineY() const { return m_baselineY; }
    float VerticalOffset() const { return m_verticalOffset; }
    float BaseY() const { return m_baseY; }
    float LineHeight() const { return m_lineHeight; }

    void Advance(float advanceX) { m_cursorX += advanceX; } // glyph 하나를 그린 뒤 advance
    bool IsLineVisible(uint32_t lineIndex, const Rect& clipRect) const; // 현재 줄이 clipRect와 겹치는지

private:
    const RenderTextItem& m_item;
    std::span<const float> m_lineWidths;

    float m_lineHeight;
    float m_ascent;
    float m_verticalOffset;
    float m_baseY;

    float m_cursorX{ 0.f };
    float m_baselineY{ 0.f };
};