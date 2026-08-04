#pragma once
#include "TextureCubeRegistry.h"

struct TextureCubeLoadRequest;
struct TextureCubeUploadEntry;
struct TextureCubeFinalizeEntry;
class TaskScheduler;
class ResourceFactory;
class DescriptorFactory;
class RenderGraph;

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
    void BuildUploadPass(RenderGraph& graph, std::vector<TextureCubeUploadEntry>& uploads, RGResourceID uploadResID);
    void BuildFinalizePass(RenderGraph& graph, std::vector<TextureCubeFinalizeEntry>& finalizeEntries);

    TaskScheduler& m_taskScheduler;
    DescriptorFactory& m_descFactory;
    ResourceFactory& m_resFactory;
    TextureCubeRegistry m_registry; // TextureRegistry와 유사하되 TextureCubeResource 대상
};