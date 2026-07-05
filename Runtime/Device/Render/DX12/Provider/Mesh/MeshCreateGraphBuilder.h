#pragma once
#include "MeshRegistry.h"

struct MeshLoadRequest;
struct MeshUploadEntry;
struct MeshFinalizeEntry;
class RenderGraph;
class TaskScheduler;
class ResourceFactory;
class DescriptorFactory;

using Microsoft::WRL::ComPtr;

class MeshCreateGraphBuilder
{
public:
    ~MeshCreateGraphBuilder();
    MeshCreateGraphBuilder() = delete;
    MeshCreateGraphBuilder(TaskScheduler& taskScheduler, ResourceFactory& resFactory, DescriptorFactory& descFactory);
    void LoadMeshes(const std::vector<MeshLoadRequest>& requests);

private:
    void BuildUploadPass(RenderGraph& graph, std::vector<MeshUploadEntry>& meshUploads, RGResourceID uploadResID);
    void BuildFinalizePass(RenderGraph& graph, std::vector<MeshFinalizeEntry>& finalizes);

    TaskScheduler& m_taskScheduler;
    ResourceFactory& m_resFactory;
    DescriptorFactory& m_descFactory;
    MeshRegistry m_registry;
};
