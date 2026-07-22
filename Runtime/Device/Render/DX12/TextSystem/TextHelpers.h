#pragma once
#include "TextTypes.h"
#include "Core/Foundation/Geometry2D.h"
#include "GameClient/Asset/MeshAsset.h"
#include "GameClient/Service/Render/Definition/Text/TextStyle.h"

GlyphInfo CreateEmptyGlyphInfo(const BitmapGlyph& glyph);
GlyphInfo CreateEmptyGlyphInfo(const SDFGlyph& glyph);

GlyphInfo CreateGlyphInfo(
    const BitmapGlyph& glyph,
    FontBucketID bucketID,
    uint16_t pageIndex,
    uint32_t packX,
    uint32_t packY,
    uint32_t padding,
    const Size& atlasSize);

GlyphInfo CreateGlyphInfo(
    const SDFGlyph& sdfGlyph,
    FontBucketID bucketID,
    uint16_t pageIndex,
    uint32_t packX,
    uint32_t packY,
    uint32_t padding,
    const Size& atlasSize);

bool CreateUploadEntry(
    GlyphPixels pixels,
    FontBucketID bucketID,
    uint16_t pageIndex,
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
    const Core::Color& color,
    UINT textureIndex);
