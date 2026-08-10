#include "pch.h"
#include "RenderBackend.h"

RenderBackend::~RenderBackend() = default;
RenderBackend::RenderBackend(const RenderConfig& config) :
    m_device{ config.enableDebugLayer },
    m_config{ config },
    m_taskScheduler{ m_cmdScheduler },
    m_swapChain{ m_cmdScheduler },
    m_descFactory{ m_device },
    m_resFactory{ m_device },
    m_resProviderSet{ m_device, m_taskScheduler, m_resFactory, m_descFactory },
    m_transientMeshProvider{ m_frameUploadPools, m_descFactory },
    m_textSystem{ m_device, m_descFactory, m_resFactory, m_transientMeshProvider },
    m_pipeline{ m_device, m_swapChain, m_descFactory, m_shaderLibrary, m_textSystem.GetBuilder() },
    m_renderFrame{ m_textSystem, m_inspector }
{}

void RenderBackend::WaitIdle()
{
    m_cmdScheduler.WaitIdle();
}

bool RenderBackend::Initialize(HWND hwnd, const Size& screenSize, std::span<const RegistryShaderDesc> registryShaders)
{
    Size shadowMapSize = { 2048, 2048 };

    ReturnIfFalse(m_cmdScheduler.Initialize(m_device, m_config.commandPools));
    SwapChainDesc desc{ hwnd, screenSize, m_config.allowTearing };
    ReturnIfFalse(m_swapChain.Initialize(m_device, desc));
    ReturnIfFalse(m_descFactory.Initialize(m_config.descriptors));
    ReturnIfFalse(m_shaderLibrary.Initialize(registryShaders));
    ReturnIfFalse(m_profiler.Initialize(m_device, m_cmdScheduler, m_resFactory));
    ReturnIfFalse(m_resProviderSet.Initialize(m_shaderLibrary));
    ReturnIfFalse(m_frameUploadPools.Initialize(m_device));
    ReturnIfFalse(m_textSystem.Initialize(m_config.text, &m_inspector));
    ReturnIfFalse(m_pipeline.Initialize(screenSize, shadowMapSize));

    return true;
}

void RenderBackend::Resize(const Size& size)
{
    m_swapChain.Resize(m_device, size);
    m_pipeline.Resize(size);
}

void RenderBackend::Update()
{
    m_taskScheduler.Execute();

    m_profiler.Update(m_frameIndex);
    float gpuMs = m_profiler.GetGpuFrameTimeMs();

    m_resProviderSet.Update(gpuMs);
}

void RenderBackend::Render()
{
    m_frameUploadPools.Reset();
    m_descFactory.GetBindlessAllocator().ResetFrameTransient();

    auto* cmd = m_cmdScheduler.Begin(CommandType::Direct);
    if (cmd)
    {
        m_profiler.BeginFrame(*cmd, m_frameIndex);
        m_pipeline.Render(*cmd, m_renderFrame.PrepareRenderData(), m_renderFrame.GetFrameData());
        m_profiler.EndFrame(*cmd);

        m_cmdScheduler.End();
        m_swapChain.Present(false);

        m_frameIndex++;
    }

    m_renderFrame.Clear();
}

RenderMetrics RenderBackend::GetRenderMetrics()
{
    RenderMetrics metrics;

    metrics.cpuFrameMs = m_profiler.GetCpuFrameTimeMs();
    metrics.gpuFrameMs = m_profiler.GetGpuFrameTimeMs();

    return metrics;
}

//////////////////////////////////////////////////////

unique_ptr<IRenderBackend> CreateRenderBackend(const RenderConfig& config)
{
	return make_unique<RenderBackend>(config);
}
