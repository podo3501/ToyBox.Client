#pragma once
#include "IMaterialResource.h"
#include "GameClient/Service/Asset/Assets/TextureAsset.h"
#include "GameClient/Service/Render/Repository/TextureDesc.h"
#include <memory>

struct IMaterialSystem
{
    virtual ~IMaterialSystem() = default;
    virtual std::shared_ptr<IMaterialResource> CreateMaterialResource() = 0;
    virtual bool LoadFromAsset(std::shared_ptr<IMaterialResource> resource, std::shared_ptr<TextureAsset> asset, const TextureDesc& desc) = 0;
};