#pragma once
#include "GameClient/Service/Render/Repository/Font/IFontProvider.h"
#include "GameClient/Service/Render/Repository/Mesh/IMeshProvider.h"
#include "GameClient/Service/Render/Repository/Material/IMaterialProvider.h"
#include "GameClient/Service/Render/Repository/Provider/IBrushProvider.h"
#include "GameClient/Service/Render/Repository/Environment/IEnvironmentProvider.h"

#include "GameClient/Service/Render/Repository/IResourceProvider.h"

struct IResourceProviderSet
{
	virtual ~IResourceProviderSet() = default;
	virtual IFontProvider* GetFontProvider() = 0;
	virtual IMeshProvider* GetMeshProvider() = 0;
	virtual IMaterialProvider* GetMaterialProvider() = 0;
	virtual IResourceProvider* GetBrushProvider() = 0;
	virtual IEnvironmentProvider* GetEnvironmentProvider() = 0;
};
