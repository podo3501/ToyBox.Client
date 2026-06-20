#pragma once
#include "GameClient/Service/Render/Repository/Mesh/IMeshProvider.h"
#include "GameClient/Service/Render/Repository/Material/IMaterialProvider.h"

struct IResourceProvider
{
	virtual ~IResourceProvider() = default;
	virtual IMeshProvider* GetMeshProvider() = 0;
	virtual IMaterialProvider* GetMaterialProvider() = 0;
};
