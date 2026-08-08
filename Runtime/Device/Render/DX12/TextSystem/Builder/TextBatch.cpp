#include "pch.h"
#include "TextBatch.h"
#include "Resource/Brush/BrushResource.h"
#include "../TextTypes.h"
#include "../Atlas/FontAtlas.h"

static BatchTarget MakeBatchTarget(PageMeshBuffer& buffer)
{
    BrushResource* brush = static_cast<BrushResource*>(buffer.brush.get());
    return { buffer, brush->GetTextureIndex() };
}

BatchTarget GetGlyphBatchTarget(
    TextBatchBufferMap& buffers,
    const GlyphInfo* glyph,
    const FontAtlas& atlas)
{
    TextBatchKey key{ glyph->bucketID, glyph->pageIndex };
    auto [it, inserted] = buffers.try_emplace(key);
    if (inserted)
        it->second.brush = atlas.GetBrush(glyph);

    return MakeBatchTarget(it->second);
}

BatchTarget GetSolidBatchTarget(
    TextBatchBufferMap& buffers,
    FontBucketID bucketID,
    const FontAtlas& atlas)
{
    TextBatchKey key{ bucketID, 0 };
    auto [it, inserted] = buffers.try_emplace(key);
    if (inserted)
        it->second.brush = atlas.GetSolidBrush();

    return MakeBatchTarget(it->second);
}