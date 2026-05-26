#pragma once
#include "GameClient/Service/Render/Desc/MaterialDesc.h"

struct IMaterialResource
{
	virtual ~IMaterialResource() = default;
	virtual bool IsReady() const noexcept = 0;
	virtual MaterialType GetType() const noexcept = 0;
};