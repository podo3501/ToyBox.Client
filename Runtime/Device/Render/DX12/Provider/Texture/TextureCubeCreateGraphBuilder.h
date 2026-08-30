#pragma once
#include "Graph/RenderGraph.h"
#include "Graph/RGResourceIDGenerator.h"
#include "TextureCubeRegistry.h"

struct TextureCubeLoadRequest;
struct TextureCubeUploadEntry;
struct TextureCubeFinalizeEntry;
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
    void BuildUploadPass(std::vector<TextureCubeUploadEntry>& uploads, RGResourceID uploadResID);
    void BuildFinalizePass(std::vector<TextureCubeFinalizeEntry>& finalizeEntries);

    TaskScheduler& m_taskScheduler;
    DescriptorFactory& m_descFactory;
    ResourceFactory& m_resFactory;

    RenderGraph m_graph;
    RGResourceIDGenerator m_idGenerator;
    TextureCubeRegistry m_registry; // TextureRegistry와 유사하되 TextureCubeResource 대상
};