#pragma once
#include "RGTypes.h"
#include "d3dx12.h"

struct MeshAsset;
struct MeshLoadRequest;
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
    void LoadMeshes(const std::vector<MeshLoadRequest>& requests);
    MeshGraphBuilder(TaskScheduler* taskScheduler, ResourceLoader* uploader,
        DescriptorFactory* descriptorFactory);

private:
    void BuildGraph(
        RenderGraph& graph,
        std::shared_ptr<MeshAsset> asset,
        RGHandle hMesh,
        RGHandle hVb, ComPtr<ID3D12Resource> vbRes,
        RGHandle hIb, ComPtr<ID3D12Resource> ibRes,
        size_t vbOffset, size_t ibOffset);

    RGHandle CreateRGHandle();

    TaskScheduler* m_taskScheduler{ nullptr };
    ResourceLoader* m_loader{ nullptr };
    DescriptorFactory* m_descriptorFactory{ nullptr };

    unique_ptr<MeshRegistry> m_registry;
    uint32_t m_nextId{ 1 };
};
