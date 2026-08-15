#pragma once
#include "SurfaceMaterialResource.h"
#include "GameClient/Service/Render/Definition/Material/PbrMaterialDe.h"

class PbrMaterialRes : public SurfaceMaterialResource
{
public:
	~PbrMaterialRes();
	PbrMaterialRes() = delete;
	PbrMaterialRes(const MaterialDesc& desc);

	virtual std::vector<BuiltinTextureBinding> GetBuiltinTextureBindings() const override;
	virtual const MaterialDesc& GetMaterialDesc() const noexcept override { return m_desc; }
	virtual SurfaceType GetSurfaceType() const noexcept override { return m_desc.surfType; }

	const PbrSurf& GetSurface() const { return m_desc.surf; }

private:
	PbrMaterialDe m_desc;
};