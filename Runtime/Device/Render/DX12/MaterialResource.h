#pragma once
#include "GameClient/Service/Render/Repository/IMaterialResource.h"
#include "GameClient/Service/Render/Repository/MaterialDesc.h"
#include "DescriptorAllocation.h"

struct ITextureResource;

class MaterialResource : public IMaterialResource
{
public:
	~MaterialResource();
	MaterialResource();
	virtual bool IsReady() const noexcept override;

	void SetAlbedoTexture(std::shared_ptr<ITextureResource> texRes);
	void SetSurface(const MaterialSurface& surface) { m_surface = surface; }
	const MaterialSurface& GetSurface() const { return m_surface; }

	DescriptorAllocation& GetAlbedoTextureSRV();

private:
	shared_ptr<ITextureResource> m_texRes;
	MaterialSurface m_surface;
};