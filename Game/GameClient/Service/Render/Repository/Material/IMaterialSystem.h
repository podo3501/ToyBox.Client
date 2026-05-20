#pragma once
#include "GameClient/Service/Asset/Assets/TextureAsset.h"
#include "GameClient/Service/Render/Desc/MaterialDesc.h"
#include "GameClient/Service/Render/Resource/IMaterialResource.h"
#include <memory>

struct IMaterialSystem
{
    virtual ~IMaterialSystem() = default;
	virtual shared_ptr<IMaterialResource> CreateMaterialResource(MaterialType materialType) = 0;
	virtual bool LoadFromAsset(
		std::shared_ptr<IMaterialResource> resource,
		std::shared_ptr<TextureAsset> texAsset,
		std::unique_ptr<MaterialDesc> matDesc) = 0;
};