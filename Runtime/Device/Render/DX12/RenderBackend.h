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
#include "Pipeline/ForwardRenderPipeline.h"
#include "BackendContext.h"

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
	virtual IBackendContext* GetBackendContext() override { return &m_backendContext; }

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
	ForwardRenderPipeline m_pipeline;
	BackendContext m_backendContext;
};