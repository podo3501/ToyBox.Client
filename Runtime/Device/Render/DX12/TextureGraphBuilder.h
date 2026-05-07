#pragma once
#include "TextureRegistry.h"

struct TextureAsset;
struct TextureLoadRequest;
struct UploadableResource;
class TaskScheduler;
class ResourceLoader;
class MipGenerator;
class DescriptorFactory;
class RenderGraph;

class TextureGraphBuilder
{
public:
    ~TextureGraphBuilder();
    TextureGraphBuilder() = delete;
    TextureGraphBuilder(TaskScheduler* taskScheduler, ResourceLoader* loader,
        MipGenerator* mipGenerator, DescriptorFactory* descriptorFactory, TextureRegistry* registry);
    //RGHandle LoadTexture(std::shared_ptr<TextureAsset> asset, const TextureDesc& desc);
    void LoadTextures(const std::vector<TextureLoadRequest>& requests);

private:
    void BuildGraph(RenderGraph& graph, std::shared_ptr<TextureAsset> asset, const TextureDesc& desc, 
        RGHandle hTex, ComPtr<ID3D12Resource> texRes, size_t offset, bool generateMips);
    RGHandle CreateRGHandle();

    TaskScheduler* m_taskScheduler{ nullptr };
    ResourceLoader* m_loader{ nullptr };
    MipGenerator* m_mipGenerator{ nullptr };
    DescriptorFactory* m_descriptorFactory{ nullptr };
    TextureRegistry* m_registry{ nullptr };

    uint32_t m_nextId{ 1 }; //?!? mesh graph builder에도 있기 때문에 나중에 하나로 합치자.
};
