#pragma once
#include "GameClient/Service/Render/IRenderBackend.h"
#include "Core/Device.h"
#include "Core/GPUProfiler.h"
#include "SwapChainPresenter.h"
#include "Command/CommandScheduler.h"
#include "Graph/TaskScheduler.h"
#include "Factory/DescriptorFactory.h"
#include "Factory/ResourceFactory.h"
#include "Resource/ResourceProviders.h"
#include "Scene/RenderScene.h"
#include "Renderer/Renderers.h"
#include "Pipeline/ForwardRenderPipeline.h"
#include "GameClient/Graphics/RenderData/DirectionalLightData.h"
#include "GameClient/Graphics/RenderData/CameraData.h"

class CommandList;

class RenderBackend : public IRenderBackend
{
public:
	~RenderBackend();
	RenderBackend(const RenderConfig& config);
	virtual bool Initialize(HWND hwnd, const Size& wndSize, const std::vector<ShaderRegisterDesc>& shaders) override;
	virtual void SetDirectionalLight(const DirectionalLightData& light) override;
	virtual void SetCamera(const CameraData& camera) override;
	virtual void DrawSurface(
		std::shared_ptr<IMeshResource> meshRes,
		std::shared_ptr<IMaterialResource> matRes,
		const Core::Math::Matrix& world) override;
	virtual void DrawUI(
		std::shared_ptr<IMeshResource> meshRes, 
		std::shared_ptr<IMaterialResource> matRes,
		const Core::Math::Matrix& world) override;
	virtual void Resize(const Size& size) override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void WaitIdle() override;
	virtual IMeshProvider* GetMeshProvider() override { return &m_resProviders.GetMeshProvider(); }
	virtual IMaterialProvider* GetMaterialProvider() override { return &m_resProviders.GetMaterialProvider(); }

private:
	bool BeginFrame();
	void EndFrame();
	void Clear(CommandList& cmd, float r, float g, float b, float a);

	Device m_device;
	RenderConfig m_config;

	CommandScheduler m_cmdScheduler;
	SwapChainPresenter m_swapChain;
	DescriptorFactory m_descFactory;
	ResourceFactory m_resFactory;
	TaskScheduler m_taskScheduler;
	GPUProfiler m_profiler;
	
	ResourceProviders m_resProviders;
	RenderScene m_scene;
	Renderers m_renderers;
	ForwardRenderPipeline m_pipeline;

	CommandList* m_cmd{ nullptr }; //direct command юс.

	Size m_size{};
	DirectionalLightData m_lightData;
	CameraData m_cameraData;
};