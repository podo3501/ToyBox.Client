#pragma once
#include "Core/RenderData.h"
#include "../Atlas/FontSetting.h"

struct ShapedText;
struct IMaterialResource;
class FontAtlas;
class TransientMeshProvider;
class TransientMeshResource;

struct PageMesh
{
    std::shared_ptr<TransientMeshResource> mesh;
    std::shared_ptr<IMaterialResource> material;
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