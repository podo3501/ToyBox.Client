#pragma once
#include "MaterialResource.h"
#include "../IPendingResource.h"
#include "GameClient/Asset/PhongSurface.h"

class TextureResource;

class PhongMaterialResource final : public MaterialResource, public IPendingResource
{
public:
	virtual ~PhongMaterialResource() override;
	PhongMaterialResource();
	virtual bool IsReady() const noexcept override { return m_ready; }
	virtual bool IsDependencyReady() const noexcept override;
	virtual void MarkReady() override { m_ready = true; }

	void SetAlbedo(std::shared_ptr<TextureResource> res) { m_albedo = std::move(res); }
	void SetNormal(std::shared_ptr<TextureResource> res) { m_normal = std::move(res); }
	void SetSurface(const PhongSurface& surface) { m_surface = surface; }

private:
	std::shared_ptr<TextureResource> m_albedo;
	std::shared_ptr<TextureResource> m_normal;
	PhongSurface m_surface;

	bool m_ready{ false };
};

