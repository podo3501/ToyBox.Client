#pragma once
#include "RGTypes.h"

struct MeshAsset;
class RenderGraph;
class TaskScheduler;
class ResourceLoader;
class DescriptorFactory;
class MeshRegistry;

class MeshGraphBuilder
{
public:
    ~MeshGraphBuilder();
    RGHandle LoadMesh(std::shared_ptr<MeshAsset> asset);
    MeshGraphBuilder(TaskScheduler* taskScheduler, ResourceLoader* uploader,
        DescriptorFactory* descriptorFactory, MeshRegistry* registry);

private:
    void BuildGraph(RenderGraph& graph, std::shared_ptr<MeshAsset> asset, 
        RGHandle vbRes, RGHandle ibRes, RGHandle meshRes);
    RGHandle CreateRGHandle();

    TaskScheduler* m_taskScheduler{ nullptr };
    ResourceLoader* m_loader{ nullptr };
    DescriptorFactory* m_descriptorFactory{ nullptr };
    MeshRegistry* m_registry{ nullptr };

    uint32_t m_nextId{ 1 };
};
