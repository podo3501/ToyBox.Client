#pragma once
#include "Graph/RenderGraph.h"
#include "Graph/RGResourceIDGenerator.h"

struct TextureCubeLoadRequest;
struct TextureCubeUploadEntry;
class TaskScheduler;
class ResourceFactory;
class DescriptorFactory;

class TextureCubeCreateGraphBuilder
{
public:
    ~TextureCubeCreateGraphBuilder();
    TextureCubeCreateGraphBuilder() = delete;
    TextureCubeCreateGraphBuilder(
        TaskScheduler& taskScheduler,
        ResourceFactory& resFactory,
        DescriptorFactory& descFactory);

    void LoadTextureCubes(const std::vector<TextureCubeLoadRequest>& requests);

private:
    std::vector<TextureCubeUploadEntry> BuildTextureCubeUploads(
        const std::vector<TextureCubeLoadRequest>& requests,
        size_t& outTotalUploadSize);

    std::shared_ptr<ResourceContext> CreateResourceContext(
        std::shared_ptr<std::vector<TextureCubeUploadEntry>> textureUploads,
        RGResourceID uploadResID,
        size_t totalUploadSize);

    void BuildUploadPass(
        std::shared_ptr<std::vector<TextureCubeUploadEntry>> textureUploads, 
        RGResourceID uploadResID);

    void FinalizeTextureCubes(std::vector<TextureCubeUploadEntry>& textureUploads);

    TaskScheduler& m_taskScheduler;
    DescriptorFactory& m_descFactory;
    ResourceFactory& m_resFactory;

    RenderGraph m_graph;
    RGResourceIDGenerator m_idGenerator;
};