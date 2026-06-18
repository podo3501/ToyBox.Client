#include "pch.h"
#include "RenderBackend.h"
#include "Command/CommandList.h"
#include "Command/CommandListHelpers.h"
#include <dxgi1_6.h>

using Microsoft::WRL::ComPtr;

RenderBackend::~RenderBackend() = default;
RenderBackend::RenderBackend(const RenderConfig& config) :
    m_device{ config.enableDebugLayer },
    m_swapChain{ m_cmdScheduler },
    m_descFactory{ m_device },
    m_resFactory{ m_device },
    m_taskScheduler{ m_cmdScheduler },
    m_resProviders{ m_device, m_descFactory, m_resFactory, m_taskScheduler },
    m_renderers{ m_device, m_resProviders.GetShaderProvider() },
    m_pipeline{ m_swapChain, m_descFactory },
    m_config{ config }
{}

void RenderBackend::WaitIdle()
{
    m_cmdScheduler.WaitIdle();
}

bool RenderBackend::Initialize(
    HWND hwnd, 
    const Size& wndSize, 
    const std::vector<ShaderRegisterDesc>& shaders)
{
    m_size = wndSize;

    ReturnIfFalse(m_cmdScheduler.Initialize(m_device, m_config.commandPools));
    SwapChainDesc desc{ hwnd, wndSize, m_config.allowTearing };
    ReturnIfFalse(m_swapChain.Initialize(m_device, desc));
    ReturnIfFalse(m_descFactory.Initialize(m_config.descriptors));
    ReturnIfFalse(m_profiler.Initialize(m_device, m_cmdScheduler, m_resFactory));

    ReturnIfFalse(m_resProviders.Initialize(shaders));
    ReturnIfFalse(m_renderers.Initialize(wndSize));
    ReturnIfFalse(m_pipeline.Initialize(m_device, Size{ 2048, 2048 }, m_renderers));
    
    return true;
}

void RenderBackend::SetCamera(const CameraData& camera)
{
    m_cameraData = camera;
}

void RenderBackend::SetDirectionalLight(const DirectionalLightData& light)
{
    m_lightData = light;
}

bool RenderBackend::BeginFrame()
{
    m_cmd = m_cmdScheduler.Begin(CommandType::Direct);
    if (m_cmd == nullptr) return false;

    return true;
}

void RenderBackend::EndFrame()
{
    assert(m_cmd);

    m_cmdScheduler.End();
    m_swapChain.Present(false);

    m_cmd = nullptr;
}

void RenderBackend::DrawSurface(
    std::shared_ptr<IMeshResource> meshRes,
    std::shared_ptr<IMaterialResource> matRes,
    const Core::Math::Matrix& world)
{   
    DrawItem item;
    item.mesh = meshRes;
    item.material = matRes;
    item.world = world;

    m_scene.AddSurface(item);
}

void RenderBackend::DrawUI(
    std::shared_ptr<IMeshResource> meshRes,
    std::shared_ptr<IMaterialResource> matRes,
    const Core::Math::Matrix& world)
{
    DrawItem item;
    item.mesh = meshRes;
    item.material = matRes;
    item.world = world;

    m_scene.AddUI(item);
}

void RenderBackend::Resize(const Size& size)
{
    m_renderers.SetScreenSize(size);
    m_swapChain.Resize(m_device, size);
}

void RenderBackend::Update()
{
    m_taskScheduler.Execute();

    m_profiler.Update();
    float gpuMs = m_profiler.GetGpuFrameTimeMs();

    m_resProviders.Update(gpuMs);
}

void RenderBackend::Render()
{
    if (!BeginFrame())
        return;

    FrameData frame;
    frame.light = m_lightData;
    frame.camera = m_cameraData;

    m_scene.SortDraws();

    m_profiler.BeginFrame(*m_cmd);
    m_pipeline.Render(*m_cmd, m_scene.BuildDrawPacket(), frame);
    m_profiler.EndFrame(*m_cmd);

    EndFrame();

    m_scene.Clear();
}

void RenderBackend::Clear(CommandList& cmd, float r, float g, float b, float a)
{
    auto rtv = m_swapChain.GetCurrentRTV();

    float color[4] = { r, g, b, a };
    CommandUtils::ClearRTV(cmd, rtv, color);
}

//////////////////////////////////////////////////////

unique_ptr<IRenderBackend> CreateRenderBackend(const RenderConfig& config)
{
	return make_unique<RenderBackend>(config);
}
