#pragma once
#include "GameClient/Service/Render/IRenderBackend.h"
#include "Core/Device.h"
#include "Command/CommandScheduler.h"
#include "Graph/TaskScheduler.h"
#include "SwapChainPresenter.h"
#include "Factory/DescriptorFactory.h"
#include "Factory/ResourceFactory.h"
#include "Shader/ShaderLibrary.h"
#include "Inspector/Inspector.h"
#include "Core/FrameProfiler.h"
#include "ResourceProviderSet.h"
#include "Allocator/FrameUploadPools.h"
#include "Provider/Mesh/TransientMeshProvider.h"
#include "Pipeline/ForwardRenderPipeline.h"
#include "TextSystem/TextSystem.h"
#include "RenderFrame.h"

class RenderBackend : public IRenderBackend
{
public:
	~RenderBackend();
	RenderBackend(const RenderConfig& config);
	virtual bool Initialize(HWND hwnd, const Size& wndSize, std::span<const RegistryShaderDesc> registryShaders) override;
	virtual ShaderID RegisterShader(const ShaderDesc& desc) override { return m_shaderLibrary.RegisterShader(desc); }
	virtual void Resize(const Size& size) override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void WaitIdle() override;

	virtual IResourceProvider* GetProvider(ProviderType type) override;
	virtual IRenderFrame* GetRenderFrame() override { return &m_renderFrame; }
	virtual RenderMetrics GetRenderMetrics() override;

private:
	Device m_device;
	RenderConfig m_config;

	CommandScheduler m_cmdScheduler;
	TaskScheduler m_taskScheduler;
	SwapChainPresenter m_swapChain;
	DescriptorFactory m_descFactory;
	ResourceFactory m_resFactory;
	ShaderLibrary m_shaderLibrary;
	Inspector m_inspector; //코드가 이상할때 조사할수 있게 도와주는 클래스
	FrameProfiler m_profiler;
	ResourceProviderSet m_resProviderSet;
	FrameUploadPools m_frameUploadPools;
	TransientMeshProvider m_transientMeshProvider;
	TextSystem m_textSystem;
	ForwardRenderPipeline m_pipeline;
	RenderFrame m_renderFrame;

	uint64_t m_frameIndex{ 0 };
};