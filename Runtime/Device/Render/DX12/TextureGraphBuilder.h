#pragma once
#include "TextureRegistry.h"

struct TextureAsset;
class TaskScheduler;
class ResourceUploader;
class MipGenerator;
class DescriptorFactory;
class RenderGraph;

class TextureGraphBuilder
{
public:
    ~TextureGraphBuilder();
    TextureGraphBuilder() = delete;
    TextureGraphBuilder(TaskScheduler* taskScheduler, ResourceUploader* uploader,
        MipGenerator* mipGenerator, DescriptorFactory* descriptorFactory, TextureRegistry* registry);
    RGResource LoadTexture(std::shared_ptr<TextureAsset> asset, const TextureDesc& desc);

private:
    void BuildGraph(RenderGraph& graph, std::shared_ptr<TextureAsset> asset,
        const TextureDesc& desc, RGResource texRes);

    TaskScheduler* m_taskScheduler{ nullptr };
    ResourceUploader* m_uploader{ nullptr };
    MipGenerator* m_mipGenerator{ nullptr };
    DescriptorFactory* m_descriptorFactory{ nullptr };
    TextureRegistry* m_registry{ nullptr };
};
