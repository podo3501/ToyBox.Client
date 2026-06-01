#pragma once
#include "GameClient/Service/Asset/Assets/TextureAsset.h"
#include "GameClient/Service/Render/Desc/MaterialDesc.h"
#include "GameClient/Service/Render/Resource/IMaterialResource.h"
#include <memory>

struct IMaterialSystem
{
    virtual ~IMaterialSystem() = default;
	virtual shared_ptr<IMaterialResource> CreateMaterialResource(const MaterialDesc& matDesc) = 0;
	virtual bool LoadFromAsset(std::shared_ptr<IMaterialResource> resource, std::vector<std::shared_ptr<TextureAsset>> texAssets) = 0;
};