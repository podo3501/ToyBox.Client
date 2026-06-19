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
    m_pipeline{ m_device, m_swapChain, m_descFactory, m_shaderLibrary },
    m_backendContext{ m_device, m_descFactory, m_resFactory, m_taskScheduler }
{}

void RenderBackend::WaitIdle()
{
    m_cmdScheduler.WaitIdle();
}

bool RenderBackend::Initialize(
    HWND hwnd, 
    const Size& screenSize, 
    const std::vector<ShaderRegisterDesc>& shaders)
{
    Size shadowMapSize = { 2048, 2048 };

    ReturnIfFalse(m_cmdScheduler.Initialize(m_device, m_config.commandPools));
    SwapChainDesc desc{ hwnd, screenSize, m_config.allowTearing };
    ReturnIfFalse(m_swapChain.Initialize(m_device, desc));
    ReturnIfFalse(m_descFactory.Initialize(m_config.descriptors));
    ReturnIfFalse(m_shaderLibrary.Initialize(shaders));
    ReturnIfFalse(m_profiler.Initialize(m_device, m_cmdScheduler, m_resFactory));
    ReturnIfFalse(m_pipeline.Initialize(screenSize, shadowMapSize));
    ReturnIfFalse(m_backendContext.Initialize(m_shaderLibrary));
    
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

    m_profiler.Update();
    float gpuMs = m_profiler.GetGpuFrameTimeMs();

    m_backendContext.Update(gpuMs);
}

void RenderBackend::Render()
{
    auto* cmd = m_cmdScheduler.Begin(CommandType::Direct);
    if (!cmd)
        return;

    m_profiler.BeginFrame(*cmd);
    m_pipeline.Render(*cmd, m_backendContext.PrepareRenderData(), m_backendContext.GetFrameData());
    m_profiler.EndFrame(*cmd);

    m_cmdScheduler.End();
    m_swapChain.Present(false);

    m_backendContext.Clear();
}

//////////////////////////////////////////////////////

unique_ptr<IRenderBackend> CreateRenderBackend(const RenderConfig& config)
{
	return make_unique<RenderBackend>(config);
}
