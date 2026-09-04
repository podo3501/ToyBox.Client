#pragma once
#include "TextBatch.h"

struct GlyphInfo;
class FontAtlas;

struct TextBatchInfo
{
    PageMeshBuffer& buffer;
    UINT texIndex;
};

TextBatchInfo GetGlyphBatchInfo(
    TextBatchBufferMap& buffers,
    const GlyphInfo* glyph,
    const FontAtlas& atlas);

TextBatchInfo GetSolidBatchInfo(
    TextBatchBufferMap& buffers,
    FontBucketID bucketID,
    const FontAtlas& atlas);
