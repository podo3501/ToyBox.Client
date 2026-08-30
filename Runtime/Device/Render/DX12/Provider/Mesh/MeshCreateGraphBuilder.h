#pragma once
#include "Graph/RenderGraph.h"
#include "Graph/RGResourceIDGenerator.h"
#include "MeshRegistry.h"

struct MeshLoadRequest;
struct MeshUploadEntry;
struct MeshFinalizeEntry;
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
    void BuildUploadPass(std::vector<MeshUploadEntry>& meshUploads, RGResourceID uploadResID);
    void BuildFinalizePass(std::vector<MeshFinalizeEntry>& finalizes);

    TaskScheduler& m_taskScheduler;
    ResourceFactory& m_resFactory;
    DescriptorFactory& m_descFactory;

    RenderGraph m_graph;
    RGResourceIDGenerator m_idGenerator;
    MeshRegistry m_registry;
};
