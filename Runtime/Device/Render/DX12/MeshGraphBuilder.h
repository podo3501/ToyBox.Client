#pragma once
#include "RGTypes.h"

struct MeshAsset;
class RenderGraph;
class TaskScheduler;
class ResourceUploader;
class DescriptorFactory;
class MeshRegistry;

class MeshGraphBuilder
{
public:
    ~MeshGraphBuilder();
    RGResource LoadMesh(std::shared_ptr<MeshAsset> asset);
    MeshGraphBuilder(TaskScheduler* taskScheduler, ResourceUploader* uploader, 
        DescriptorFactory* descriptorFactory, MeshRegistry* registry);

private:
    void BuildGraph(RenderGraph& graph, std::shared_ptr<MeshAsset> asset, RGResource texRes);

    TaskScheduler* m_taskScheduler{ nullptr };
    ResourceUploader* m_uploader{ nullptr };
    DescriptorFactory* m_descriptorFactory{ nullptr };
    MeshRegistry* m_registry{ nullptr };
};
