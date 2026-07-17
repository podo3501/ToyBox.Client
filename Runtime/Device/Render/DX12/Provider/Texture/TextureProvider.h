#pragma once
#include "TextureLoadRequest.h"
#include "TextureCreateGraphBuilder.h"
#include "Resource/Texture/BuiltinTextureType.h"
#include <queue>

class Device;
class BindlessDescriptorAllocator;
class DescriptorFactory;
class TaskScheduler;
class ResourceFactory;
class ShaderLibrary;
namespace Core { struct Color; }

class TextureProvider
{
public:
    ~TextureProvider();
    TextureProvider() = delete;
    explicit TextureProvider(TextureCreateGraphBuilder create) noexcept;
    shared_ptr<TextureResource> CreateResource(const TextureDesc& desc);
    bool LoadResource(std::shared_ptr<TextureResource> resource, std::shared_ptr<TextureAsset> asset);

    bool Initialize(ShaderLibrary& shaderLibrary);
    void Update(size_t uploadBudgetBytes);
    std::shared_ptr<TextureResource> GetBuiltinTexture(BuiltinTextureType type) const;

private:
    bool CreateBuiltinTextures();
    std::shared_ptr<TextureResource> CreateBuiltinTexture(const TextureDesc& desc, std::shared_ptr<TextureAsset> asset);
    std::shared_ptr<TextureAsset> CreateColorAsset(const Core::Color& color);

    TextureCreateGraphBuilder m_createBuilder;
    std::queue<TextureLoadRequest> m_pending;
    std::array<std::shared_ptr<TextureResource>, Core::EnumSize<BuiltinTextureType>> m_builtinTextures;
};