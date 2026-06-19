#pragma once
#include "GameClient/Service/Render/IBackendContext.h"
#include "Provider/ResourceProviders.h"
#include "Scene/RenderScene.h"

class BackendContext : public IBackendContext
{
public:
	~BackendContext();
	BackendContext(
		Device& device,
		DescriptorFactory& descFactory,
		ResourceFactory& resFactory,
		TaskScheduler& taskScheduler);

	virtual void SetFrameData(const FrameData& frameData) noexcept override;
	virtual void DrawSurface(
		std::shared_ptr<IMeshResource> meshRes,
		std::shared_ptr<IMaterialResource> matRes,
		const Core::Math::Matrix& world) override;
	virtual void DrawUI(
		std::shared_ptr<IMeshResource> meshRes,
		std::shared_ptr<IMaterialResource> matRes,
		const Core::Math::Matrix& world) override;

	virtual IMeshProvider* GetMeshProvider() override { return &m_resProviders.GetMeshProvider(); }
	virtual IMaterialProvider* GetMaterialProvider() override { return &m_resProviders.GetMaterialProvider(); }

	bool Initialize(ShaderLibrary& shaderLibrary);
	DrawPacket PrepareRenderData();
	void Update(float gpuMs);
	void Clear();

	const FrameData& GetFrameData() const { return m_frameData; }

private:
	ResourceProviders m_resProviders;
	RenderScene m_scene;
	FrameData m_frameData;
};