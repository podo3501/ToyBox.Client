#pragma once
#include "Graph/RGUploadIDAllocator.h"
#include "Graph/RenderGraph.h"
#include "TextureRegistry.h"
#include "MipGenerator.h"

struct TextureLoadRequest;
struct TextureUploadEntry;
struct TextureFinalizeEntry;
class Device;
class TaskScheduler;
class ResourceFactory;
class DescriptorFactory;
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
    void BuildUploadPass(std::vector<TextureUploadEntry>& textureUploads, RGResourceID uploadResID);
    void BuildMipPass(std::vector<TextureUploadEntry>& textureUploads);
    void BuildFinalizePass(std::vector<TextureFinalizeEntry>& finalizeEntries, RGResourceID uploadResID);

    TaskScheduler& m_taskScheduler;
    MipGenerator m_mipGenerator;
    DescriptorFactory& m_descFactory;
    ResourceFactory& m_resFactory;
    RGUploadIDAllocator m_idAllocator;

    RenderGraph m_graph;
    TextureRegistry m_registry;
};
