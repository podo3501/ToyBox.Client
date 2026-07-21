#pragma once
#include "DebugSurfaceMaterialResource.h"
#include "GameClient/Service/Render/Definition/Material/GridMaterialDesc.h"

class GridMaterialResource : public DebugSurfaceMaterialResource
{
public:
	~GridMaterialResource();
	GridMaterialResource() = delete;
	GridMaterialResource(const MaterialDesc& desc);

	virtual const MaterialDesc& GetMaterialDesc() const noexcept override { return m_desc; }
	virtual DebugSurfaceType GetDebugSurfaceType() const noexcept override { return m_desc.debugSurfType; }

private:
	GridMaterialDesc m_desc;
};

