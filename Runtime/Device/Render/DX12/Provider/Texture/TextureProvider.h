#pragma once
#include "../IUpdatableProvider.h"
#include "TextureLoadRequest.h"
#include "TextureCreateGraphBuilder.h"
#include "../PendingUploadQueue.h"
#include "Resource/Texture/BuiltinTextureType.h"
#include <queue>

class Device;
class BindlessDescriptorAllocator;
class DescriptorFactory;
class TaskScheduler;
class ResourceFactory;
class ShaderLibrary;
namespace Core { struct Color; }

class TextureProvider : public IUpdatableProvider
{
public:
    ~TextureProvider();
    TextureProvider() = delete;
    explicit TextureProvider(TextureCreateGraphBuilder create) noexcept;
    virtual void Update(float avgGpuMs) override;

    shared_ptr<TextureResource> CreateResource();
    bool LoadResource(std::shared_ptr<TextureResource> resource, std::shared_ptr<TextureAsset> asset);
    bool Initialize(ShaderLibrary& shaderLibrary);

    std::shared_ptr<TextureResource> GetBuiltinTexture(BuiltinTextureType type) const;

private:
    bool CreateBuiltinTextures();
    std::shared_ptr<TextureResource> CreateBuiltinTexture(std::shared_ptr<TextureAsset> asset);

    TextureCreateGraphBuilder m_createBuilder;
    PendingUploadQueue<TextureLoadRequest> m_pendingLoads;
    std::array<std::shared_ptr<TextureResource>, Core::EnumSize<BuiltinTextureType>> m_builtinTextures;
};