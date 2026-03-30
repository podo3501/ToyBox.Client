#include "pch.h"
#include "RenderBackend.h"
#include "DX12Core.h"
#include "DX12Device.h"
#include "FrameDisplay.h"
#include "CommandScheduler.h"
#include "QuadRenderer.h"
#include "d3dx12.h"
#include <dxgi1_6.h>

RenderBackend::~RenderBackend() = default;
RenderBackend::RenderBackend() :
    m_device{ make_unique<DX12Device>() }
{}

bool RenderBackend::Initialize(HWND hwnd, const Size& wndSize, const RenderConfig& config)
{
    m_size = wndSize;
    ReturnIfFalse(m_device->Initialize(config.enableDebugLayer));

    m_frameDisplay = make_unique<FrameDisplay>(m_device->GetCore());
    ReturnIfFalse(m_frameDisplay->Initialize(hwnd, wndSize, config.allowTearing));

    m_commandScheduler = make_unique<CommandScheduler>(m_device->GetCore());
    ReturnIfFalse(m_commandScheduler->Initialize());

    m_quadRenderer = make_unique<QuadRenderer>(m_device->GetCore());
    ReturnIfFalse(m_quadRenderer->Initialize());

    return true;
}

bool RenderBackend::BeginFrame()
{
    ReturnIfFalse(m_commandScheduler->BeginFrame());
    m_frameDisplay->BindCurrentRTV(m_commandScheduler->GetCommandList());

    return true;
}

bool RenderBackend::EndFrame()
{
    ReturnIfFalse(m_commandScheduler->EndFrame());
    ReturnIfFalse(m_frameDisplay->Present(false));

    return true;
}

//bool RenderBackend::Present(bool vsync)
//{
//    return m_frameDisplay->Present(vsync);
//}

bool RenderBackend::Render()
{
    ReturnIfFalse(BeginFrame());

    auto cmd = m_commandScheduler->GetCommandList();
    m_quadRenderer->BindPipeline(cmd);
    m_quadRenderer->Draw(cmd);

    return EndFrame();
}

void RenderBackend::Clear(float r, float g, float b, float a)
{
    auto cmd = m_commandScheduler->GetCommandList();
    auto rtv = m_frameDisplay->GetCurrentRTV();

    float color[4] = { r, g, b, a };
    cmd->ClearRenderTargetView(rtv, color, 0, nullptr);
}

//void RenderBackend::BeginFrame()
//{
//    m_deviceResources->BeginFrame();
//}
//
//void RenderBackend::Clear(float r, float g, float b, float a)
//{
//    auto cmd = m_deviceResources->GetCommandList();
//    auto rtv = m_deviceResources->GetCurrentRTV();
//
//    float color[4] = { r, g, b, a };
//    cmd->ClearRenderTargetView(rtv, color, 0, nullptr);
//}
//
//void RenderBackend::EndFrame()
//{
//    m_deviceResources->EndFrame();
//}
//
//void RenderBackend::Present(bool vsync)
//{
//    m_deviceResources->Present(vsync);
//}
//
//void RenderBackend::Render()
//{ 
//    m_deviceResources->BeginFrame();
//
//    auto cmd = m_deviceResources->GetCommandList();
//
//    m_deviceResources->BindQuadPipeline(cmd);
//    m_deviceResources->DrawQuad(cmd);
//
//    m_deviceResources->EndFrame();
//    m_deviceResources->Present(false); 
//}

//////////////////////////////////////////////////////

unique_ptr<IRenderBackend> CreateRenderBackend()
{
	return make_unique<RenderBackend>();
}
