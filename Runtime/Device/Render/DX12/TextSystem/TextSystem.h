#pragma once
#include "Builder/FontAtlasUploadGraphBuilder.h"
#include "Atlas/FontAtlas.h"

struct TextConfig;
struct ShapedText;
struct RenderTextItem;
struct UIBatchBuffer;
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
        UIBatchBuffer& buffer);

    FontAtlasUploadGraphBuilder& GetBuilder() { return m_atlasBuilder; }

private:
    std::vector<ShapedText> ShapeTexts(std::span<const RenderTextItem> items);

    FontAtlasUploadGraphBuilder m_atlasBuilder;
    FontAtlas m_fontAtlas;
};
