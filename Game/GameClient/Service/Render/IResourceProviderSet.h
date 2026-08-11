#pragma once
#include "GameClient/Service/Render/Repository/Mesh/IMeshProvider.h"
#include "GameClient/Service/Render/Repository/Material/IMaterialProvider.h"

#include "GameClient/Service/Render/Repository/IResourceProvider.h"

struct IResourceProviderSet
{
	virtual ~IResourceProviderSet() = default;
	virtual IResourceProvider* GetFontProvider() = 0;
	virtual IMeshProvider* GetMeshProvider() = 0;
	virtual IMaterialProvider* GetMaterialProvider() = 0;
	virtual IResourceProvider* GetBrushProvider() = 0;
	virtual IResourceProvider* GetEnvironmentProvider() = 0;
};
