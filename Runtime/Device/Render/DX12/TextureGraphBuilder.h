#pragma once
#include "RGTypes.h"
#include "d3dx12.h"

struct TextureAsset;
struct TextureDesc;
struct TextureLoadRequest;
struct UploadableResource;
struct ID3D12Resource;
class TextureRegistry;
class TaskScheduler;
class ResourceLoader;
class MipGenerator;
class DescriptorFactory;
class RenderGraph;

using Microsoft::WRL::ComPtr;

class TextureGraphBuilder
{
public:
    ~TextureGraphBuilder();
    TextureGraphBuilder() = delete;
    TextureGraphBuilder(TaskScheduler* taskScheduler, ResourceLoader* loader,
        MipGenerator* mipGenerator, DescriptorFactory* descriptorFactory);
    void LoadTextures(const std::vector<TextureLoadRequest>& requests);

private:
    void BuildGraph(RenderGraph& graph, std::shared_ptr<TextureAsset> asset, const TextureDesc& desc, 
        RGHandle hTex, ComPtr<ID3D12Resource> texRes, size_t offset, bool generateMips);
    RGHandle CreateRGHandle();

    TaskScheduler* m_taskScheduler{ nullptr };
    ResourceLoader* m_loader{ nullptr };
    MipGenerator* m_mipGenerator{ nullptr };
    DescriptorFactory* m_descriptorFactory{ nullptr };

    unique_ptr<TextureRegistry> m_registry;
    uint32_t m_nextId{ 1 }; //?!? mesh graph builder에도 있기 때문에 나중에 하나로 합치자.
};
