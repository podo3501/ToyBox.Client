#pragma once
#include "GameClient/Asset/TextureAsset.h"
#include "GameClient/Service/Render/Desc/MaterialDesc.h"
#include "GameClient/Service/Render/Resource/IMaterialResource.h"
#include <memory>

struct IMaterialProvider
{
    virtual ~IMaterialProvider() = default;
	virtual shared_ptr<IMaterialResource> CreateResource(const MaterialDesc& matDesc) = 0;
	virtual bool LoadResource(std::shared_ptr<IMaterialResource> resource,
		std::unordered_map<TextureSlot, std::shared_ptr<TextureAsset>> texAssets) = 0;
	virtual void ReleaseResource(std::shared_ptr<IMaterialResource> resource) = 0;
};