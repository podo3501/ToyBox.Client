#pragma once
#include "TextureRegistry.h"

struct TextureLoadRequest;
struct TextureUploadEntry;
struct TextureFinalizeEntry;
class TaskScheduler;
class ResourceFactory;
class MipGenerator;
class DescriptorFactory;
class RenderGraph;

using Microsoft::WRL::ComPtr;

class TextureGraphBuilder
{
public:
    ~TextureGraphBuilder();
    TextureGraphBuilder() = delete;
    TextureGraphBuilder(TaskScheduler& taskScheduler, ResourceFactory& resFactory,
        MipGenerator& mipGenerator, DescriptorFactory& descFactory);

    void LoadTextures(const std::vector<TextureLoadRequest>& requests);

private:
    void BuildUploadPass(RenderGraph& graph, std::vector<TextureUploadEntry>& textureUploads, RGResourceID uploadResID);
    void BuildMipPass(RenderGraph& graph, std::vector<TextureUploadEntry>& textureUploads);
    void BuildFinalizePass(RenderGraph& graph, std::vector<TextureFinalizeEntry>& finalizeEntries);

    TaskScheduler& m_taskScheduler;
    MipGenerator& m_mipGenerator;
    DescriptorFactory& m_descFactory;
    ResourceFactory& m_resFactory;
    TextureRegistry m_registry;
};
