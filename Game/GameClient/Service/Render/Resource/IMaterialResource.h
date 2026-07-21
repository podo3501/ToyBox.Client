#pragma once
#include "IResource.h"
#include "GameClient/Service/Render/Definition/Material/MaterialDesc.h"

struct IMaterialResource : public IResource
{
	virtual ~IMaterialResource() = default;
};