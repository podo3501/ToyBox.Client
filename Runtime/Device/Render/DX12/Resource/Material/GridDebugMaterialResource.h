#pragma once
#include "DebugMaterialResource.h"

class GridDebugMaterialResource final : public DebugMaterialResource
{
public:
	GridDebugMaterialResource();
	virtual bool IsReady() const noexcept override { return true; }
};