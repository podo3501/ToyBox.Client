#pragma once
#include "GameClient/Service/Render/Repository/Font/IFontProvider.h"
#include "GameClient/Service/Render/Repository/Mesh/IMeshProvider.h"
#include "GameClient/Service/Render/Repository/Material/IMaterialProvider.h"
#include "GameClient/Service/Render/Repository/Provider/IBrushProvider.h"
#include "GameClient/Service/Render/Repository/Environment/IEnvironmentProvider.h"

struct IResourceProvider
{
	virtual ~IResourceProvider() = default;
	virtual IFontProvider* GetFontProvider() = 0;
	virtual IMeshProvider* GetMeshProvider() = 0;
	virtual IMaterialProvider* GetMaterialProvider() = 0;
	virtual IBrushProvider* GetBrushProvider() = 0;
	virtual IEnvironmentProvider* GetEnvironmentProvider() = 0;
};
