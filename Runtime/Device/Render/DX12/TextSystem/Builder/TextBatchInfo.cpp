#include "pch.h"
#include "TextBatchInfo.h"
#include "../Atlas/FontAtlas.h"
#include "Resource/Brush/BrushResource.h"

static TextBatchInfo MakeBatchTarget(PageMeshBuffer& buffer)
{
    BrushResource* brush = static_cast<BrushResource*>(buffer.brush.get());
    return { buffer, brush->GetTextureIndex() };
}

TextBatchInfo GetGlyphBatchInfo(
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

TextBatchInfo GetSolidBatchInfo(
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