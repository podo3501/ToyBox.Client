#include "pch.h"
#include "MeshBuilderHelpers.h"
#include "Core/Utils/BitUtils.h"
#include "GameClient/Asset/MeshAsset.h"
#include "Core/RenderData.h"
#include "TextBatch.h"

std::vector<PackedTextParams> PackRunParams(const std::vector<TextRun>& runs)
{
    std::vector<PackedTextParams> packed;
    packed.reserve(runs.size());
    for (const auto& run : runs)
    {
        const auto& style = run.style;
        packed.push_back(
            {
                Core::PackNibbles(
                    style.outline.value_or(TextOutline{}),
                    style.shadow.value_or(TextShadow{}),
                    style.gradient.value_or(TextGradient{})),
                Core::PackNibbles(style.glow.value_or(TextGlow{}))
            });
    }
    return packed;
}

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

static void AppendQuadIndices(std::vector<uint32_t>& indices, uint32_t vertexOffset)
{
    indices.insert(indices.end(), {
        vertexOffset + 0, vertexOffset + 1, vertexOffset + 2,
        vertexOffset + 0, vertexOffset + 2, vertexOffset + 3
        });
}

void AppendGlyphQuad(
    BatchTarget& target,
    const GlyphInfo& glyph,
    float x,
    float y,
    const Core::Color& color,
    PackedTextParams packParams,
    const Rect& clipRect)
{
    float x1 = x + glyph.width;
    float y1 = y + glyph.height;

    auto mode = ToUIRenderMode(glyph.mode);
    auto& vertices = target.buffer.vertices;
    auto& indices = target.buffer.indices;
    auto textureIndex = target.texIndex;

    UITextProps textProps
    {
        .sdfPxRange = glyph.pxRange,
        .clipRect = clipRect,
        .params1 = packParams.params1,
        .params2 = packParams.params2
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

    auto& vertexOffset = target.buffer.vertexOffset;
    AppendQuadIndices(indices, vertexOffset);
    vertexOffset += 4;
}

void AppendSolidQuad(
    BatchTarget& target,
    const Rect& rect,
    const Core::Color& color,
    const Rect& clipRect)
{
    float x = rect.x;
    float y = rect.y;
    float x1 = x + rect.width;
    float y1 = y + rect.height;

    auto& vertices = target.buffer.vertices;
    auto& indices = target.buffer.indices;
    auto textureIndex = target.texIndex;

    UITextProps textProps{ .sdfPxRange = 0.f, .clipRect = clipRect, .params1 = 0, .params2 = 0 };

    vertices.push_back({ { x,  y,  0.f }, color, { 0.f, 0.f }, textureIndex, UIRenderMode::UI, textProps });
    vertices.push_back({ { x1, y,  0.f }, color, { 1.f, 0.f }, textureIndex, UIRenderMode::UI, textProps });
    vertices.push_back({ { x1, y1, 0.f }, color, { 1.f, 1.f }, textureIndex, UIRenderMode::UI, textProps });
    vertices.push_back({ { x,  y1, 0.f }, color, { 0.f, 1.f }, textureIndex, UIRenderMode::UI, textProps });

    auto& vertexOffset = target.buffer.vertexOffset;
    AppendQuadIndices(indices, vertexOffset);
    vertexOffset += 4;
}