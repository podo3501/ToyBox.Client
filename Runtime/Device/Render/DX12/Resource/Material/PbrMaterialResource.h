#pragma once
#include "MaterialResource.h"
#include "../IPendingResource.h"
#include "GameClient/Asset/PbrSurface.h"

struct PbrSurface;
class TextureResource;

class PbrMaterialResource final : public MaterialResource, public IPendingResource
{
public:
	virtual ~PbrMaterialResource() override;
	PbrMaterialResource();
	virtual bool IsReady() const noexcept override { return m_ready; }
	virtual bool IsDependencyReady() const noexcept override;
	virtual void MarkReady() override { m_ready = true; }

	void SetAlbedo(std::shared_ptr<TextureResource> res) { m_albedo = std::move(res); }
	void SetNormal(std::shared_ptr<TextureResource> res) { m_normal = std::move(res); }
	void SetArm(std::shared_ptr<TextureResource> res) { m_arm = std::move(res); }
	void SetSurface(const PbrSurface& surface) { m_surface = surface; }

private:
	std::shared_ptr<TextureResource> m_albedo;
	std::shared_ptr<TextureResource> m_normal;
	std::shared_ptr<TextureResource> m_arm;
	PbrSurface m_surface;

	bool m_ready{ false };
};

