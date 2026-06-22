#pragma once
#include "GameClient/Service/Render/IRenderBackend.h"
#include "Core/Device.h"
#include "Command/CommandScheduler.h"
#include "Graph/TaskScheduler.h"
#include "SwapChainPresenter.h"
#include "Factory/DescriptorFactory.h"
#include "Factory/ResourceFactory.h"
#include "Shader/ShaderLibrary.h"
#include "Core/GPUProfiler.h"
#include "ResourceProvider.h"
#include "Pipeline/ForwardRenderPipeline.h"
#include "RenderFrame.h"

class RenderBackend : public IRenderBackend
{
public:
	~RenderBackend();
	RenderBackend(const RenderConfig& config);
	virtual bool Initialize(HWND hwnd, const Size& wndSize, const std::vector<ShaderRegisterDesc>& shaders) override;
	virtual void Resize(const Size& size) override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void WaitIdle() override;
	virtual IResourceProvider* GetResourceProvider() override { return &m_resProvider; }
	virtual IRenderFrame* GetRenderFrame() override { return &m_renderFrame; }

private:
	Device m_device;
	RenderConfig m_config;

	CommandScheduler m_cmdScheduler;
	TaskScheduler m_taskScheduler;
	SwapChainPresenter m_swapChain;
	DescriptorFactory m_descFactory;
	ResourceFactory m_resFactory;
	ShaderLibrary m_shaderLibrary;
	GPUProfiler m_profiler;
	ResourceProvider m_resProvider;
	ForwardRenderPipeline m_pipeline;
	RenderFrame m_renderFrame;
};