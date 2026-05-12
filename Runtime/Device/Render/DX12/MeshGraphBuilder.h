#pragma once
#include "RGTypes.h"
#include "d3dx12.h"

struct MeshAsset;
struct MeshLoadRequest;
struct MeshUploadEntry;
struct MeshFinalizeEntry;
class RenderGraph;
class TaskScheduler;
class ResourceLoader;
class DescriptorFactory;
class MeshRegistry;

using Microsoft::WRL::ComPtr;

class MeshGraphBuilder
{
public:
    ~MeshGraphBuilder();
    MeshGraphBuilder() = delete;
    MeshGraphBuilder(TaskScheduler* taskScheduler, ResourceLoader* uploader,
        DescriptorFactory* descriptorFactory);

    void LoadMeshes(const std::vector<MeshLoadRequest>& requests);

private:
    void BuildUploadPass(RenderGraph& graph, std::vector<MeshUploadEntry>& meshUploads);
    void BuildFinalizePass(RenderGraph& graph, std::vector<MeshFinalizeEntry>& finalizes);

    RGHandle CreateRGHandle();

    TaskScheduler* m_taskScheduler{ nullptr };
    ResourceLoader* m_loader{ nullptr };
    DescriptorFactory* m_descriptorFactory{ nullptr };

    unique_ptr<MeshRegistry> m_registry;
    uint32_t m_nextId{ 1 };
};
