#pragma once
#include "TextureRegistry.h"
#include "MipGenerator.h"

struct TextureLoadRequest;
struct TextureUploadEntry;
struct TextureFinalizeEntry;
class Device;
class TaskScheduler;
class ResourceFactory;
class DescriptorFactory;
class RenderGraph;
class ShaderLibrary;

using Microsoft::WRL::ComPtr;

class TextureCreateGraphBuilder
{
public:
    ~TextureCreateGraphBuilder();
    TextureCreateGraphBuilder() = delete;
    TextureCreateGraphBuilder(
        Device& device,
        TaskScheduler& taskScheduler, 
        ResourceFactory& resFactory,
        DescriptorFactory& descFactory);

    bool Initialize(ShaderLibrary& shaderLibrary);
    void LoadTextures(const std::vector<TextureLoadRequest>& requests);

private:
    void BuildUploadPass(RenderGraph& graph, std::vector<TextureUploadEntry>& textureUploads, RGResourceID uploadResID);
    void BuildMipPass(RenderGraph& graph, std::vector<TextureUploadEntry>& textureUploads);
    void BuildFinalizePass(RenderGraph& graph, std::vector<TextureFinalizeEntry>& finalizeEntries);

    TaskScheduler& m_taskScheduler;
    MipGenerator m_mipGenerator;
    DescriptorFactory& m_descFactory;
    ResourceFactory& m_resFactory;
    TextureRegistry m_registry;
};
