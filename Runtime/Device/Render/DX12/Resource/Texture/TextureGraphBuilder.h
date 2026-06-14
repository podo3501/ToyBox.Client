#pragma once
#include "Graph/RGTypes.h"
#include "d3dx12.h"

struct TextureLoadRequest;
struct TextureUploadEntry;
struct TextureFinalizeEntry;
class TextureRegistry;
class TaskScheduler;
class ResourceFactory;
class MipGenerator;
class DescriptorFactory;
class RenderGraph;

using Microsoft::WRL::ComPtr;

class TextureGraphBuilder
{
public:
    ~TextureGraphBuilder();
    TextureGraphBuilder() = delete;
    TextureGraphBuilder(TaskScheduler* taskScheduler, ResourceFactory* resFactory,
        MipGenerator* mipGenerator, DescriptorFactory* descFactory);

    void LoadTextures(const std::vector<TextureLoadRequest>& requests);

private:
    void BuildUploadPass(RenderGraph& graph, std::vector<TextureUploadEntry>& textureUploads, RGHandle hUploadRes);
    void BuildMipPass(RenderGraph& graph, std::vector<TextureUploadEntry>& textureUploads);
    void BuildFinalizePass(RenderGraph& graph, std::vector<TextureFinalizeEntry>& finalizeEntries);

    RGHandle CreateRGHandle();

    TaskScheduler* m_taskScheduler{ nullptr };
    ResourceFactory* m_resFactory{ nullptr };
    MipGenerator* m_mipGenerator{ nullptr };
    DescriptorFactory* m_descFactory{ nullptr };

    unique_ptr<TextureRegistry> m_registry;
    uint32_t m_nextId{ 1 }; //?!? mesh graph builder에도 있기 때문에 나중에 하나로 합치자.
};
