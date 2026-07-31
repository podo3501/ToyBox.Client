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

    float startX = m_startX;
    float useWidth = width;
    float useY = underlineY;
    float useHeight = m_thickness;

    // clipRect와 교집합해서 실제로 보일 영역만큼만 quad를 만든다.
    // 완전히 밖이면 아예 push하지 않음.
    Rect underlineRect{ startX, underlineY, width, m_thickness };
    if (!underlineRect.Intersects(m_clipRect))
        return;

    Rect clipped = underlineRect.Intersect(m_clipRect);
    startX = clipped.x;
    useWidth = clipped.width;
    useY = clipped.y;
    useHeight = clipped.height;

    TextBatchKey key{ SolidQuadBucket, 0 };
    auto target = GetOrCreateBatchTarget(m_buffers, key, m_atlas.GetSolidMaterial());

    AppendSolidQuad(
        target.buffer.vertices, 
        target.buffer.indices, 
        target.buffer.vertexOffset,
        startX, useY, useWidth, useHeight, 
        target.texIndices[0], m_color,
        m_clipRect); // 이제 clipRect 안에 완전히 들어가는 quad라 셰이더 discard는 안전망 역할만
}