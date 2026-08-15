#pragma once
#include "SurfaceMaterialResource.h"
#include "GameClient/Service/Render/Definition/Material/PhongMaterialDe.h"

class PhongMaterialRes : public SurfaceMaterialResource
{
public:
	~PhongMaterialRes();
	PhongMaterialRes() = delete;
	PhongMaterialRes(const MaterialDesc& desc);

	virtual std::vector<BuiltinTextureBinding> GetBuiltinTextureBindings() const override;
	virtual const MaterialDesc& GetMaterialDesc() const noexcept override { return m_desc; }
	virtual SurfaceType GetSurfaceType() const noexcept override { return m_desc.surfType; }

	const PhongSurf& GetSurface() const { return m_desc.surf; }

private:
	PhongMaterialDe m_desc;
};