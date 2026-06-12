#pragma once
#include "GameClient/Service/Asset/Assets/TextureAsset.h"
#include "GameClient/Service/Render/Desc/TextureDesc.h"
#include "GameClient/Service/Render/Resource/ITextureResource.h"

struct ITextureProvider
{
    virtual ~ITextureProvider() = default;
    virtual shared_ptr<ITextureResource> CreateTextureResource(const TextureDesc& desc) = 0;
    virtual bool LoadFromAsset(std::shared_ptr<ITextureResource> resource, std::shared_ptr<TextureAsset> asset) = 0;
};