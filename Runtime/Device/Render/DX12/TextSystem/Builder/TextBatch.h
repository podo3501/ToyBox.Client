#pragma once
#include "../Atlas/FontSetting.h"
#include "Core/Utils/Hash.h"
#include "GameClient/Asset/MeshAsset.h"

struct UIVertex;
struct GlyphInfo;
class FontAtlas;
class BrushResource;

struct PageMeshBuffer
{
    std::vector<UIVertex> vertices;
    std::vector<uint32_t> indices;
    uint32_t vertexOffset = 0;

    std::shared_ptr<BrushResource> brush{ nullptr };
};

struct TextBatchKey
{
    FontBucketID bucket{ InvalidFontBucket };
    uint16_t pageIndex{ 0 };

    bool operator==(const TextBatchKey&) const = default;
};

struct TextBatchKeyHash
{
    size_t operator()(const TextBatchKey& key) const
    {
        return Core::HashOf(
            key.bucket,
            key.pageIndex);
    }
};

using TextBatchBufferMap = std::unordered_map<TextBatchKey, PageMeshBuffer, TextBatchKeyHash>;

struct BatchTarget
{
    PageMeshBuffer& buffer;
    UINT texIndex;
};

BatchTarget GetGlyphBatchTarget(
    TextBatchBufferMap& buffers,
    const GlyphInfo* glyph,
    const FontAtlas& atlas);

BatchTarget GetSolidBatchTarget(
    TextBatchBufferMap& buffers,
    FontBucketID bucketID,
    const FontAtlas& atlas);
