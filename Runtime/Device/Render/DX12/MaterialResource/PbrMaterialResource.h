#pragma once
#include "MaterialResource.h"
#include "GameClient/Service/Render/Desc/PbrMaterialDesc.h"

class PbrMaterialResource : public MaterialResource
{
public:
	~PbrMaterialResource();
	PbrMaterialResource() = delete;
	PbrMaterialResource(const MaterialDesc& desc);

	virtual std::vector<DefaultTextureType> GetRequiredDefaultTextures() const override;
	virtual const MaterialDesc& GetMaterialDesc() const noexcept override { return m_desc; }

	const PbrSurface& GetSurface() const { return m_desc.surf; }

private:
	PbrMaterialDesc m_desc;
};