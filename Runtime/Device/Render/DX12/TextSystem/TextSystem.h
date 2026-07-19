#pragma once
#include "Atlas/FontAtlas.h"
#include "Builder/FontAtlasUploadGraphBuilder.h"
#include "Builder/TextMeshBuilder.h"

struct PageMesh;
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
        TaskScheduler& taskScheduler, 
        ResourceFactory& resFactory,
        TransientMeshProvider& transientMeshProvider);
    bool Initialize(const Size& atlasTexSize, Inspector* inspector = nullptr);
    std::vector<DrawUIItem> BuildDrawItems(std::span<const DrawTextItem> items);

private:
    std::vector<ShapedText> ShapeTexts(std::span<const DrawTextItem> items);
    void UploadPendingGlyphs(std::span<const ShapedText> shapedTexts);
    std::vector<DrawUIItem> CreateDrawItems(std::span<const PageMesh> pageMeshes);

    FontAtlas m_fontAtlas;
    FontAtlasUploadGraphBuilder m_atlasBuilder;
    TextMeshBuilder m_meshBuilder;

    Inspector* m_inspector{ nullptr };
};
