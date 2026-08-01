#include "pch.h"
#include "TextBatch.h"
#include "Resource/Material/UIMaterialResource.h"
#include "../TextTypes.h"
#include "../Atlas/FontAtlas.h"

static BatchTarget MakeBatchTarget(PageMeshBuffer& buffer)
{
    UIMaterialResource* uiMat = static_cast<UIMaterialResource*>(buffer.material.get());
    return { buffer, uiMat->GetTextureIndices() };
}

BatchTarget GetGlyphBatchTarget(
    TextBatchBufferMap& buffers,
    const GlyphInfo* glyph,
    const FontAtlas& atlas)
{
    TextBatchKey key{ glyph->bucketID, glyph->pageIndex };
    auto [it, inserted] = buffers.try_emplace(key);
    if (inserted)
        it->second.material = atlas.GetMaterial(glyph);

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
        it->second.material = atlas.GetSolidMaterial();

    return MakeBatchTarget(it->second);
}