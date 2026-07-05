#pragma once
#include "IResource.h"
#include "GameClient/Service/Render/Desc/MaterialDesc.h"

struct IMaterialResource : public IResource
{
	virtual ~IMaterialResource() = default;
};