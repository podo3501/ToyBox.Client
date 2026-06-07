#pragma once
#include "SurfaceMaterialResource.h"
#include "GameClient/Service/Render/Desc/PhongMaterialDesc.h"

class PhongMaterialResource : public SurfaceMaterialResource
{
public:
	~PhongMaterialResource();
	PhongMaterialResource() = delete;
	PhongMaterialResource(const MaterialDesc& desc);

	virtual std::vector<DefaultTextureType> GetRequiredDefaultTextures() const override;
	virtual const MaterialDesc& GetMaterialDesc() const noexcept override { return m_desc; }
	virtual SurfaceType GetSurfaceType() const noexcept override { return m_desc.surfType; }

	const PhongSurface& GetSurface() const { return m_desc.surf; }

private:
	PhongMaterialDesc m_desc;
};