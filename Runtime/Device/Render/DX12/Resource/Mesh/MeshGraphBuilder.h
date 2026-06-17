#pragma once
#include "Graph/RGTypes.h"
#include "d3dx12.h"

struct MeshAsset;
struct MeshLoadRequest;
struct MeshUploadEntry;
struct MeshFinalizeEntry;
class RenderGraph;
class TaskScheduler;
class ResourceFactory;
class DescriptorFactory;
class MeshRegistry;

using Microsoft::WRL::ComPtr;

class MeshGraphBuilder
{
public:
    ~MeshGraphBuilder();
    MeshGraphBuilder() = delete;
    MeshGraphBuilder(TaskScheduler* taskScheduler, ResourceFactory& resFactory, DescriptorFactory& descFactory);
    void LoadMeshes(const std::vector<MeshLoadRequest>& requests);

private:
    void BuildUploadPass(RenderGraph& graph, std::vector<MeshUploadEntry>& meshUploads, RGHandle hUploadRes);
    void BuildFinalizePass(RenderGraph& graph, std::vector<MeshFinalizeEntry>& finalizes);

    RGHandle CreateRGHandle();

    TaskScheduler* m_taskScheduler{ nullptr };
    ResourceFactory& m_resFactory;
    DescriptorFactory& m_descFactory;

    unique_ptr<MeshRegistry> m_registry;
    uint32_t m_nextId{ 1 };
};
