#pragma once
#include "../TextTypes.h"
#include "Core/Foundation/Geometry2D.h"

struct UIVertex;

void AppendGlyphQuad(
    std::vector<UIVertex>& vertices,
    std::vector<uint32_t>& indices,
    uint32_t& vertexOffset,
    const GlyphInfo& glyph,
    float x,
    float y,
    UINT textureIndex,
    const TextStyle& style,
    const Rect& clipRect);

void AppendSolidQuad(
    std::vector<UIVertex>& vertices,
    std::vector<uint32_t>& indices,
    uint32_t& vertexOffset,
    float x, float y,
    float width, float height,
    UINT textureIndex,
    const Core::Color& color,
    const Rect& clipRect);
