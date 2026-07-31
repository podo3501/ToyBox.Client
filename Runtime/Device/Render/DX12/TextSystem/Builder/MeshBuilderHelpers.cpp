#include "pch.h"
#include "MeshBuilderHelpers.h"
#include "Core/Utils/BitUtils.h"
#include "GameClient/Asset/MeshAsset.h"

static UIRenderMode ToUIRenderMode(TextRenderMode mode)
{
    switch (mode)
    {
    case TextRenderMode::MTSDF: return UIRenderMode::MTSDF;
    case TextRenderMode::Bitmap: return UIRenderMode::BitmapText;
    }
    Assert(false);

    return UIRenderMode::MTSDF;
}

void AppendGlyphQuad(
    std::vector<UIVertex>& vertices,
    std::vector<uint32_t>& indices,
    uint32_t& vertexOffset,
    const GlyphInfo& glyph,
    float x,
    float y,
    UINT textureIndex,
    const TextStyle& style,
    const Rect& clipRect)
{
    const Core::Color& color = style.color;
    float x1 = x + glyph.width;
    float y1 = y + glyph.height;
    auto mode = ToUIRenderMode(glyph.mode);
    auto params1 = Core::PackNibbles(
        style.outline.value_or(TextOutline{}),
        style.shadow.value_or(TextShadow{}),
        style.gradient.value_or(TextGradient{}));
    auto params2 = Core::PackNibbles(
        style.glow.value_or(TextGlow{}));

    UITextProps textProps
    {
        .sdfPxRange = glyph.pxRange,
        .clipRect = clipRect,
        .params1 = params1,
        .params2 = params2
    };

    vertices.push_back(
        {
            { x, y, 0.f },
            color,
            { glyph.uvMin.x, glyph.uvMin.y },
            textureIndex,
            mode,
            textProps
        });

    vertices.push_back(
        {
            { x1, y, 0.f },
            color,
            { glyph.uvMax.x, glyph.uvMin.y },
            textureIndex,
            mode,
            textProps
        });

    vertices.push_back(
        {
            { x1, y1, 0.f },
            color,
            { glyph.uvMax.x, glyph.uvMax.y },
            textureIndex,
            mode,
            textProps
        });

    vertices.push_back(
        {
            { x, y1, 0.f },
            color,
            { glyph.uvMin.x, glyph.uvMax.y },
            textureIndex,
            mode,
            textProps
        });

    indices.insert(indices.end(), {
        vertexOffset + 0, vertexOffset + 1, vertexOffset + 2,
        vertexOffset + 0, vertexOffset + 2, vertexOffset + 3
        });

    vertexOffset += 4;
}

void AppendSolidQuad(
    std::vector<UIVertex>& vertices,
    std::vector<uint32_t>& indices,
    uint32_t& vertexOffset,
    float x, float y,
    float width, float height,
    UINT textureIndex,
    const Core::Color& color,
    const Rect& clipRect)
{
    float x1 = x + width;
    float y1 = y + height;

    UITextProps textProps{ .sdfPxRange = 0.f, .clipRect = clipRect, .params1 = 0, .params2 = 0 };

    vertices.push_back({ { x,  y,  0.f }, color, { 0.f, 0.f }, textureIndex, UIRenderMode::UI, textProps });
    vertices.push_back({ { x1, y,  0.f }, color, { 1.f, 0.f }, textureIndex, UIRenderMode::UI, textProps });
    vertices.push_back({ { x1, y1, 0.f }, color, { 1.f, 1.f }, textureIndex, UIRenderMode::UI, textProps });
    vertices.push_back({ { x,  y1, 0.f }, color, { 0.f, 1.f }, textureIndex, UIRenderMode::UI, textProps });

    indices.insert(indices.end(), {
        vertexOffset + 0, vertexOffset + 1, vertexOffset + 2,
        vertexOffset + 0, vertexOffset + 2, vertexOffset + 3
        });

    vertexOffset += 4;
}