#include "pch.h"
#include "UnderlineBatcher.h"
#include "../Atlas/FontAtlas.h"
#include "../TextTypes.h"
#include "Resource/Font/FontResource.h"
#include "MeshBuilderHelpers.h"
#include "TextBatch.h"

UnderlineBatcher::UnderlineBatcher(
    const FontAtlas& atlas, 
    const ShapedText& shaped, 
    TextBatchBufferMap& buffers,
    const Rect& clipRect) :
    m_atlas{ atlas }, m_shaped{ shaped }, m_buffers{ buffers }, m_clipRect{ clipRect }
{}

void UnderlineBatcher::Update(const TextStyle& style, float cursorX, float baselineY)
{
    bool wantsUnderline = style.underline.has_value();
    Core::Color wantsColor = wantsUnderline
        ? style.underline->color.value_or(style.color)
        : Core::Color{};
    float wantsThickness = wantsUnderline
        ? (style.underline->thickness > 0.f
            ? style.underline->thickness
            : m_shaped.font->GetUnderlineThickness(m_shaped.size))
        : 0.f;

    bool changed = m_active &&
        (!wantsUnderline || wantsColor != m_color || wantsThickness != m_thickness);

    if (changed)
        Flush(cursorX, baselineY);

    if (!m_active && wantsUnderline)
    {
        m_active = true;
        m_startX = cursorX;
        m_color = wantsColor;
        m_thickness = wantsThickness;
    }
}

void UnderlineBatcher::Flush(float endX, float baselineY)
{
    if (!m_active)
        return;
    m_active = false;

    float width = endX - m_startX;
    if (width <= 0.f || m_thickness <= 0.f)
        return;

    float underlineY = baselineY + m_shaped.font->GetUnderlineOffset(m_shaped.size);
    Rect underlineRect{ m_startX, underlineY, width, m_thickness };
    if (m_clipRect.IsValid())
    {
        if (!underlineRect.Intersects(m_clipRect)) // clipRect와 교집합해서 실제로 보일 영역만큼만 quad를 만든다. 완전히 밖이면 아예 push하지 않음.
            return;
        underlineRect = underlineRect.Intersect(m_clipRect);
    }

    auto target = GetSolidBatchTarget(m_buffers, SolidQuadBucket, m_atlas);
    AppendSolidQuad(
        target,
        underlineRect,
        m_color,
        m_clipRect); // clipRect가 유효할 때는 이미 이 안에 완전히 들어가는 quad이므로 셰이더 discard는 안전망 역할만.
}