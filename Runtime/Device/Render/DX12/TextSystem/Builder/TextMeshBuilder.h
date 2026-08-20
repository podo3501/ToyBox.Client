#pragma once
#include "Core/RenderData.h"
#include "../Atlas/FontSetting.h"

struct ShapedText;
class BrushResource;
class FontAtlas;
class TransientMeshProvider;
class TransientMeshResource;

struct PageMesh
{
    std::shared_ptr<TransientMeshResource> mesh;
    std::shared_ptr<BrushResource> brush;
};

class TextMeshBuilder
{
public:
    explicit TextMeshBuilder(TransientMeshProvider& meshProvider);

    std::vector<PageMesh> Build(
        const FontAtlas& atlas,
        std::span<const RenderTextItem> items,
        std::span<const ShapedText> shapedTexts);

private:
    TransientMeshProvider& m_meshProvider;
};