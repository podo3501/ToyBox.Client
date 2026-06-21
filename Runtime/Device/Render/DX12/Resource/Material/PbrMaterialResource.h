#pragma once
#include "SurfaceMaterialResource.h"
#include "GameClient/Service/Render/Desc/PbrMaterialDesc.h"

class PbrMaterialResource : public SurfaceMaterialResource
{
public:
	~PbrMaterialResource();
	PbrMaterialResource() = delete;
	PbrMaterialResource(const MaterialDesc& desc);

	virtual std::vector<DefaultTextureBinding> GetDefaultTextureBindings() const override;
	virtual const MaterialDesc& GetMaterialDesc() const noexcept override { return m_desc; }
	virtual SurfaceType GetSurfaceType() const noexcept override { return m_desc.surfType; }

	const PbrSurface& GetSurface() const { return m_desc.surf; }

private:
	PbrMaterialDesc m_desc;
};