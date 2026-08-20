#pragma once
#include "Builder/FontAtlasUploadGraphBuilder.h"
#include "Builder/TextMeshBuilder.h"
#include "Atlas/FontAtlas.h"

template<typename T>
concept FontAtlasType = std::derived_from<T, FontAtlas>;

struct PageMesh;
struct TextConfig;
struct ShapedText; 
class Device;
class TaskScheduler;
class ResourceFactory;
class DescriptorFactory;
class TransientMeshProvider;
class Inspector;

class TextSystem
{
public:
    ~TextSystem();
    TextSystem(
        Device& device,
        DescriptorFactory& factory,
        ResourceFactory& resFactory,
        TransientMeshProvider& transientMeshProvider);
    bool Initialize(const TextConfig& texConfig, Inspector* inspector = nullptr);
    std::vector<RenderUIItem> BuildDrawItems(std::span<const RenderTextItem> items);

    FontAtlasUploadGraphBuilder& GetBuilder() { return m_atlasBuilder; }

private:
    std::vector<ShapedText> ShapeTexts(std::span<const RenderTextItem> items);
    std::vector<RenderUIItem> CreateDrawItems(std::span<const PageMesh> pageMeshes);

    FontAtlasUploadGraphBuilder m_atlasBuilder;
    TextMeshBuilder m_meshBuilder;
    FontAtlas m_fontAtlas;

    Inspector* m_inspector{ nullptr };
};
