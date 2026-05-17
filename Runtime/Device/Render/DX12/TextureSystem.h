#pragma once
#include "GameClient/Service/Render/Repository/ITextureSystem.h"
#include "TextureLoadRequest.h"
#include <queue>

struct ID3D12Device;
class DescriptorAllocator;
class MipGenerator;
class DescriptorFactory;
class TextureGraphBuilder;
class TaskScheduler;
class ResourceLoader;

class TextureSystem : public ITextureSystem
{
public:
    ~TextureSystem();
    TextureSystem(ID3D12Device* device, DescriptorAllocator* srvAllocator, TaskScheduler* taskScheduler, ResourceLoader* loader);
    virtual shared_ptr<ITextureResource> CreateTextureResource() override;
    virtual bool LoadFromAsset(std::shared_ptr<ITextureResource> resource, std::shared_ptr<TextureAsset> asset, const TextureDesc& desc) override;

    bool Initialize();
    void Update(size_t uploadBudgetBytes);
    std::shared_ptr<ITextureResource> GetDefaultTexture() const { return m_defaultTexture; }

private:
    bool CreateBuiltinTextures();
    std::shared_ptr<TextureAsset> CreateDefaultTextureAsset();

    unique_ptr<MipGenerator> m_mipGenerator;
    unique_ptr<DescriptorFactory> m_descriptorFactory;
    unique_ptr<TextureGraphBuilder> m_builder;

    std::queue<TextureLoadRequest> m_pending;

    std::shared_ptr<ITextureResource> m_defaultTexture;
    std::shared_ptr<TextureAsset> m_defaultAsset;
};