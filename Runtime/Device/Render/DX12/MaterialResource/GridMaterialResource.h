#pragma once
#include "MaterialResource.h"
#include "GameClient/Service/Render/Desc/GridMaterialDesc.h"

class GridMaterialResource : public MaterialResource
{
public:
	~GridMaterialResource();
	GridMaterialResource() = delete;
	GridMaterialResource(const MaterialDesc& desc);

	virtual const MaterialDesc& GetMaterialDesc() const noexcept override { return m_desc; }

private:
	GridMaterialDesc m_desc;
};

