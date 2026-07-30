#include "pch.h"
#include "UnderlineBatcher.h"
#include "../Atlas/FontAtlas.h"
#include "../TextTypes.h"
#include "Resource/Font/FontResource.h"
#include "../TextHelpers.h"

UnderlineBatcher::UnderlineBatcher(const FontAtlas& atlas, const ShapedText& shaped, TextBatchBufferMap& buffers) :
    m_atlas{ atlas }, m_shaped{ shaped }, m_buffers{ buffers }
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

    TextBatchKey key{ SolidQuadBucket, 0 };
    auto& buf = m_buffers[key];
    if (!buf.material)
        buf.material = m_atlas.GetSolidMaterial();

    UIMaterialResource* uiMat = static_cast<UIMaterialResource*>(buf.material.get());
    auto texIndices = uiMat->GetTextureIndices();

    AppendSolidQuad(
        buf.vertices, buf.indices, buf.vertexOffset,
        m_startX, underlineY, width, m_thickness,
        texIndices[0], m_color);
}