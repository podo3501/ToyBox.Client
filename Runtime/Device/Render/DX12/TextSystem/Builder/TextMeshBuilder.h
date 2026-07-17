#pragma once
#include "Core/RenderData.h"
#include "../Atlas/AtlasTypes.h"

struct ShapedText;
class FontAtlas;
class TransientMeshProvider;
class TransientMeshResource;

struct PageMesh
{
    FontBucketID bucket{ InvalidFontBucket };
    uint16_t pageIndex{ 0 };
    std::shared_ptr<TransientMeshResource> mesh;
};

class TextMeshBuilder
{
public:
    explicit TextMeshBuilder(TransientMeshProvider& meshProvider);

    std::vector<PageMesh> Build(
        const FontAtlas& atlas,
        std::span<const DrawTextItem> items,
        std::span<const ShapedText> shapedTexts);

private:
    TransientMeshProvider& m_meshProvider;
};