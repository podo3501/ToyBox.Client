#pragma once
#include "GameClient/Service/Render/Repository/ITextureSystem.h"

struct ID3D12Device;
class DescriptorAllocator;
class MipGenerator;
class DescriptorFactory;
class TextureGraphBuilder;
class TextureRegistry;
class TaskScheduler;
class ResourceUploader;

class TextureSystem : public ITextureSystem
{
public:
    ~TextureSystem();
    TextureSystem(ID3D12Device* device, DescriptorAllocator* srvAllocator, TaskScheduler* taskScheduler, ResourceUploader* uploader);
    virtual shared_ptr<ITextureResource> CreateTextureResource() override;
    virtual bool LoadFromAsset(std::shared_ptr<ITextureResource> resource, std::shared_ptr<TextureAsset> asset, const TextureDesc& desc) override;

    bool Initialize();

private:
    unique_ptr<MipGenerator> m_mipGenerator;
    unique_ptr<DescriptorFactory> m_descriptorFactory;
    unique_ptr<TextureRegistry> m_registry;
    unique_ptr<TextureGraphBuilder> m_builder;
};