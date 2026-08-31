#pragma once
#include "Graph/RenderGraph.h"
#include "Graph/RGResourceIDGenerator.h"

struct MeshLoadRequest;
struct MeshUploadEntry;
class TaskScheduler;
class ResourceFactory;
class DescriptorFactory;

using Microsoft::WRL::ComPtr;

class MeshCreateGraphBuilder
{
public:
    ~MeshCreateGraphBuilder();
    MeshCreateGraphBuilder() = delete;
    MeshCreateGraphBuilder(
        TaskScheduler& taskScheduler, 
        ResourceFactory& resFactory, 
        DescriptorFactory& descFactory);

    void LoadMeshes(const std::vector<MeshLoadRequest>& requests);

private:
    std::vector<MeshUploadEntry> BuildMeshUploads(
        const std::vector<MeshLoadRequest>& requests,
        size_t& outTotalUploadSize);

    std::shared_ptr<ResourceContext> CreateResourceContext(
        std::shared_ptr<std::vector<MeshUploadEntry>> meshUploads,
        RGResourceID uploadResID,
        size_t totalUploadSize);

    void BuildUploadPass(
        std::shared_ptr<std::vector<MeshUploadEntry>> meshUploads,
        RGResourceID uploadResID);

    void FinalizeMeshes(std::vector<MeshUploadEntry>& meshUploads);

    TaskScheduler& m_taskScheduler;
    ResourceFactory& m_resFactory;
    DescriptorFactory& m_descFactory;

    RenderGraph m_graph;
    RGResourceIDGenerator m_idGenerator;
};
