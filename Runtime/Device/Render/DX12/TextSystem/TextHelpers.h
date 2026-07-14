#pragma once
#include "TextTypes.h"
#include "Core/Foundation/Geometry2D.h"
#include "GameClient/Asset/MeshAsset.h"

struct FT_GlyphSlotRec_;
using FT_GlyphSlot = FT_GlyphSlotRec_*;

GlyphInfo CreateEmptyGlyphInfo(FT_GlyphSlot slot);

GlyphInfo CreateGlyphInfo(
    FT_GlyphSlot slot,
    uint32_t packX,
    uint32_t packY,
    uint32_t padding,
    const Size& atlasSize,
    uint16_t pageIndex);

bool CreateUploadEntry(
    FT_GlyphSlot slot,
    uint32_t packX,
    uint32_t packY,
    uint32_t padding,
    GlyphUploadEntry& outEntry);

void AppendGlyphQuad(
    std::vector<UIVertex>& vertices,
    std::vector<uint32_t>& indices,
    uint32_t& vertexOffset,
    const GlyphInfo& glyph,
    float x,
    float y,
    const Core::Color& color);
