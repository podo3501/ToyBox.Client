#pragma once
#include "Graph/RGResourceIDGenerator.h"
#include "Graph/RenderGraph.h"
#include "MipGenerator.h"

struct TextureLoadRequest;
struct TextureUploadEntry;
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
    std::vector<TextureUploadEntry> BuildTextureUploads(
        const std::vector<TextureLoadRequest>& requests,
        size_t& outTotalUploadSize,
        bool& outHasMipTask);

    std::shared_ptr<ResourceContext> CreateResourceContext(
        std::shared_ptr<std::vector<TextureUploadEntry>> textureUploads,
        RGResourceID uploadResID,
        size_t totalUploadSize);
    
    void BuildUploadPass(
        std::shared_ptr<std::vector<TextureUploadEntry>> textureUploads,
        RGResourceID uploadResID);
    void BuildMipPass(std::shared_ptr<std::vector<TextureUploadEntry>> textureUploads);
    void FinalizeTextures(std::vector<TextureUploadEntry>& textureUploads);

    TaskScheduler& m_taskScheduler;
    MipGenerator m_mipGenerator;
    DescriptorFactory& m_descFactory;
    ResourceFactory& m_resFactory;

    RGResourceIDGenerator m_idGenerator;
    RenderGraph m_graph;
};
