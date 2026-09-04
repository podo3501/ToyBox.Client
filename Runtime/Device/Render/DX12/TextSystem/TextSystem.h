#pragma once
#include "Builder/FontAtlasUploadGraphBuilder.h"
#include "Atlas/FontAtlas.h"
#include "Builder/TextBatch.h"

struct TextConfig;
struct ShapedText;
struct RenderTextItem;
class Device;
class ResourceFactory;
class DescriptorFactory;

class TextSystem
{
public:
    ~TextSystem();
    TextSystem(
        Device& device,
        DescriptorFactory& factory,
        ResourceFactory& resFactory);
    bool Initialize(const TextConfig& texConfig);
    void AppendDrawItems(
        std::span<const RenderTextItem> items,
        TextBatchBufferMap& buffers);

    FontAtlasUploadGraphBuilder& GetBuilder() { return m_atlasBuilder; }

private:
    std::vector<ShapedText> ShapeTexts(std::span<const RenderTextItem> items);

    FontAtlasUploadGraphBuilder m_atlasBuilder;
    FontAtlas m_fontAtlas;
};
