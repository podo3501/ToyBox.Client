#pragma once
#include "GameClient/Service/Render/Repository/Texture/ITextureProvider.h"
#include "TextureLoadRequest.h"
#include <queue>

class Device;
class DescriptorAllocator;
class MipGenerator;
class DescriptorFactory;
class TextureGraphBuilder;
class TaskScheduler;
class ResourceLoader;
class ShaderProvider;

enum class DefaultTextureType
{
    White,  // 일반 컬러/알베도용 (1,1,1,1)
    FlatNormal, // 노멀 맵용 (0.5, 0.5, 1.0)
    Orange, //arm용 오렌지색(1, 0.5, 0)
    Black, // 필요시 (0,0,0,1)
    Count
};

class TextureProvider : public ITextureProvider
{
public:
    ~TextureProvider();
    TextureProvider(Device& device, DescriptorFactory* descFactory, TaskScheduler* taskScheduler, ResourceLoader* loader);
    virtual shared_ptr<ITextureResource> CreateTextureResource(const TextureDesc& desc) override;
    virtual bool LoadFromAsset(std::shared_ptr<ITextureResource> resource, std::shared_ptr<TextureAsset> asset) override;

    bool Initialize(ShaderProvider* shaderProvider);
    void Update(size_t uploadBudgetBytes);
    std::shared_ptr<ITextureResource> GetDefaultTexture(DefaultTextureType type) const;

private:
    bool CreateBuiltinTextures();
    std::shared_ptr<ITextureResource> CreateDefaultTexture(const TextureDesc& desc, std::shared_ptr<TextureAsset> asset);
    std::shared_ptr<TextureAsset> CreateColorAsset(uint32_t pixelColor);

    unique_ptr<MipGenerator> m_mipGenerator;
    unique_ptr<TextureGraphBuilder> m_builder;

    std::queue<TextureLoadRequest> m_pending;

    std::unordered_map<DefaultTextureType, std::shared_ptr<TextureAsset>> m_defaultAssets;
    std::unordered_map<DefaultTextureType, std::shared_ptr<ITextureResource>> m_defaultTextures;
};