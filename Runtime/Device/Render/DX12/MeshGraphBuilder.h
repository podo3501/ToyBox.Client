#pragma once
#include "RGTypes.h"

struct MeshAsset;
class RenderGraph;
class TaskScheduler;
class ResourceUploader;

class MeshGraphBuilder
{
public:
    ~MeshGraphBuilder();
    RGResource LoadMesh(std::shared_ptr<MeshAsset> asset);
    MeshGraphBuilder(TaskScheduler* taskScheduler, ResourceUploader* uploader);

private:
    void BuildGraph(RenderGraph& graph, std::shared_ptr<MeshAsset> asset, RGResource texRes);

    TaskScheduler* m_taskScheduler{ nullptr };
    ResourceUploader* m_uploader{ nullptr };
};
