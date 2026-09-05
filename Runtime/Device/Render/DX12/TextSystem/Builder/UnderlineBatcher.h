#pragma once
#include "Core/Foundation/Geometry2D.h"
#include "Core/Foundation/Color.h"

struct TextStyle;
struct UIBatchBuffer;
struct ShapedText;
class FontAtlas;

class UnderlineBatcher
{
public:
    UnderlineBatcher(
        const FontAtlas& atlas,
        const ShapedText& shaped,
        UIBatchBuffer& buffer,
        const Rect& clipRect);

    void Update(
        const TextStyle& style,
        float cursorX,
        float baselineY); // 현재 glyph의 style을 보고, 필요시 구간을 열거나 닫는다.

    void Flush(
        float endX,
        float baselineY); // 진행 중인 구간을 quad로 확정하고 닫는다. (줄 전환, 순회 종료 시에도 호출)    

private:
    const FontAtlas& m_atlas;
    const ShapedText& m_shaped;
    UIBatchBuffer& m_buffer;
    Rect m_clipRect;

    bool m_active{ false };
    float m_startX{ 0.f };

    Core::Color m_color{};
    float m_thickness{ 0.f };
};