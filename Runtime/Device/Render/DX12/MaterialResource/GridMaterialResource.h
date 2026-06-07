#pragma once
#include "SurfaceMaterialResource.h"
#include "GameClient/Service/Render/Desc/GridMaterialDesc.h"

class GridMaterialResource : public SurfaceMaterialResource
{
public:
	~GridMaterialResource();
	GridMaterialResource() = delete;
	GridMaterialResource(const MaterialDesc& desc);

	virtual const MaterialDesc& GetMaterialDesc() const noexcept override { return m_desc; }
	virtual SurfaceType GetSurfaceType() const noexcept override { return m_desc.surfType; }

private:
	GridMaterialDesc m_desc;
};

