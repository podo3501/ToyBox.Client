#pragma once
#include "GameClient/Service/Render/Resource/IResource.h"

class GridDebugMaterialResource : public IResource
{
public:
	virtual ~GridDebugMaterialResource() override;
	GridDebugMaterialResource();
	virtual bool IsReady() const noexcept override { return true; }
};