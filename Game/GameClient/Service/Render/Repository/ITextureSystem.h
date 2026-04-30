#pragma once
#include "GameClient/Service/Asset/Assets/TextureAsset.h"
#include "GameClient/Service/Render/Repository/TextureDesc.h"
#include "ITextureResource.h"

struct ITextureSystem
{
    virtual ~ITextureSystem() = default;
    virtual shared_ptr<ITextureResource> CreateTextureResource() = 0;
    virtual bool LoadFromAsset(std::shared_ptr<ITextureResource> resource, std::shared_ptr<TextureAsset> asset, const TextureDesc& desc) = 0;
};