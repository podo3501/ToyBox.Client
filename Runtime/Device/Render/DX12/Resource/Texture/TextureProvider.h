#pragma once
#include "TextureLoadRequest.h"
#include <queue>

class Device;
class DescriptorAllocator;
class MipGenerator;
class DescriptorFactory;
class TextureGraphBuilder;
class TaskScheduler;
class ResourceFactory;
class ShaderProvider;

enum class DefaultTextureType
{
    White,  // 일반 컬러/알베도용 (1,1,1,1)
    FlatNormal, // 노멀 맵용 (0.5, 0.5, 1.0)
    Orange, //arm용 오렌지색(1, 0.5, 0)
    Black, // 필요시 (0,0,0,1)
    Count
};

class TextureProvider
{
public:
    ~TextureProvider();
    TextureProvider(Device& device, DescriptorFactory& descFactory, TaskScheduler* taskScheduler, ResourceFactory& resFactory);
    shared_ptr<TextureResource> CreateTextureResource(const TextureDesc& desc);
    bool LoadFromAsset(std::shared_ptr<TextureResource> resource, std::shared_ptr<TextureAsset> asset);

    bool Initialize(ShaderProvider* shaderProvider);
    void Update(size_t uploadBudgetBytes);
    std::shared_ptr<TextureResource> GetDefaultTexture(DefaultTextureType type) const;

private:
    bool CreateBuiltinTextures();
    std::shared_ptr<TextureResource> CreateDefaultTexture(const TextureDesc& desc, std::shared_ptr<TextureAsset> asset);
    std::shared_ptr<TextureAsset> CreateColorAsset(uint32_t pixelColor);

    unique_ptr<MipGenerator> m_mipGenerator;
    unique_ptr<TextureGraphBuilder> m_builder;

    std::queue<TextureLoadRequest> m_pending;

    std::unordered_map<DefaultTextureType, std::shared_ptr<TextureAsset>> m_defaultAssets;
    std::unordered_map<DefaultTextureType, std::shared_ptr<TextureResource>> m_defaultTextures;
};