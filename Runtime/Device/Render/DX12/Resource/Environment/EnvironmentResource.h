#pragma once
#include "GameClient/Service/Render/Resource/IEnvironmentResource.h"
#include "Core/Math/Vector3.h"

class TextureCubeResource;

class EnvironmentResource : public IEnvironmentResource
{
public:
	virtual ~EnvironmentResource() override;
	EnvironmentResource();
	virtual bool IsReady() const noexcept override { return m_ready; }
	void MarkReady() { m_ready = true; }
	bool IsTextureReady() const noexcept;

	void SetSkybox(std::shared_ptr<TextureCubeResource> res) { m_skybox = std::move(res); }
	void SetReflection(std::shared_ptr<TextureCubeResource> res) { m_reflection = std::move(res); }
	void SetIrradianceSH(const std::array<Core::Vector3, 9>& sh) { m_irradianceSH = sh; }

	std::shared_ptr<TextureCubeResource> GetSkybox() const { return m_skybox; }
	std::shared_ptr<TextureCubeResource> GetReflection() const { return m_reflection; }
	std::array<Core::Vector3, 9> GetIrradianceSH() const { return m_irradianceSH; }

private:
	std::shared_ptr<TextureCubeResource> m_skybox;
	std::shared_ptr<TextureCubeResource> m_reflection;
	std::array<Core::Vector3, 9> m_irradianceSH{};
	bool m_ready{ false };
};
