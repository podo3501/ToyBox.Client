#include "pch.h"
#include "SwapChainPresenter.h"
#include "Core/Device.h"
#include "Command/CommandScheduler.h"
#include "Command/CommandList.h"
#include "Command/CommandListHelpers.h"
#include <dxgi1_6.h>

using Microsoft::WRL::ComPtr;

SwapChainPresenter::~SwapChainPresenter() { m_cmdScheduler.WaitIdle(CommandType::Direct); }
SwapChainPresenter::SwapChainPresenter(CommandScheduler& cmdScheduler) :
    m_cmdScheduler{ cmdScheduler }
{}

bool SwapChainPresenter::Initialize(Device& device, const SwapChainDesc& desc)
{
    m_frameCount = desc.frameCount;
    m_renderTargets.resize(m_frameCount);

    m_size = desc.size;
    m_tearing = desc.allowTearing;

    auto queue = m_cmdScheduler.GetCommandQueue(CommandType::Direct);
    ReturnIfFalse(CreateSwapChain(device, queue, desc));
    ReturnIfFalse(CreateRTV(device));
    ReturnIfFalse(CreateDepthBuffer(device));

    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    return true;
}

void SwapChainPresenter::Clear(CommandList& cmd, float r, float g, float b, float a)
{
    float color[4] = { r, g, b, a };
    CommandUtils::ClearRTV(cmd, GetCurrentRTV(), color);
    CommandUtils::ClearDSV(cmd, GetDSV());
}

void SwapChainPresenter::SetRenderTarget(CommandList& cmd)
{
    CommandUtils::SetRenderTarget(cmd, GetCurrentRTV(), GetDSV());
    CommandUtils::SetViewport(cmd, static_cast<float>(m_size.width), static_cast<float>(m_size.height));
    CommandUtils::SetScissor(cmd, m_size.width, m_size.height);
}

void SwapChainPresenter::TransitionToRenderTarget(CommandList& cmd)
{
    CommandUtils::Transition(cmd, m_renderTargets[m_frameIndex],
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void SwapChainPresenter::TransitionToPresent(CommandList& cmd)
{
    CommandUtils::Transition(cmd, m_renderTargets[m_frameIndex],
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);
}

bool SwapChainPresenter::Present(bool vsync)
{
    UINT syncInterval = vsync ? 1 : 0;
    UINT flags = (!vsync && m_tearing) ? DXGI_PRESENT_ALLOW_TEARING : 0;
    if (FAILED(m_swapChain->Present(syncInterval, flags))) return false;

    m_cmdScheduler.SignalQueue(CommandType::Direct);
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
    return true;
}

bool SwapChainPresenter::Resize(Device& device, const Size& size)
{
    if (size.width == 0 || size.height == 0) return false;
    if (m_size == size) return true;

    m_cmdScheduler.WaitIdle(); // GPU 작업 끝날 때까지 대기

    for (UINT i = 0; i < m_frameCount; ++i)
        m_renderTargets[i].Reset(); //기존 RTV 리소스 해제
    m_depthBuffer.Reset();
    m_dsvHeap.Reset();

    DXGI_SWAP_CHAIN_DESC desc{}; //SwapChain Resize
    if (FAILED(m_swapChain->GetDesc(&desc)))
        return false;

    UINT flags = desc.Flags;
    if (m_tearing)
        flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    else
        flags &= ~DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    if (FAILED(m_swapChain->ResizeBuffers(
        m_frameCount,
        size.width,
        size.height,
        desc.BufferDesc.Format,
        flags)))
        return false;

    m_size = size;
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    ReturnIfFalse(CreateFrameRTVs(device));
    ReturnIfFalse(CreateDepthBuffer(device));

    return true;
}

D3D12_CPU_DESCRIPTOR_HANDLE SwapChainPresenter::GetCurrentRTV() const
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE handle(
        m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
    return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE SwapChainPresenter::GetDSV() const
{
    return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
}

bool SwapChainPresenter::CreateSwapChain(Device& device, ID3D12CommandQueue* queue, const SwapChainDesc& desc)
{
    DXGI_SWAP_CHAIN_DESC1 scDesc{};
    scDesc.BufferCount = m_frameCount;
    scDesc.Width = desc.size.width;
    scDesc.Height = desc.size.height;
    scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scDesc.SampleDesc.Count = 1;
    scDesc.Flags = desc.allowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
    
    auto factory = device.GetFactory();
    if (!factory) return false;

    ComPtr<IDXGISwapChain1> swapChain1;
    if (FAILED(factory->CreateSwapChainForHwnd(
        queue,
        desc.hwnd,
        &scDesc,
        nullptr,
        nullptr,
        &swapChain1)))
        return false;

    factory->MakeWindowAssociation(desc.hwnd, DXGI_MWA_NO_ALT_ENTER);

    return SUCCEEDED(swapChain1.As(&m_swapChain));
}

bool SwapChainPresenter::CreateRTV(Device& device)
{
    m_rtvHeap = device.CreateDescriptorHeap(
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
        m_frameCount,
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

    m_rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    
    return CreateFrameRTVs(device);
}

bool SwapChainPresenter::CreateFrameRTVs(Device& device)
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

    for (UINT i = 0; i < m_frameCount; ++i)
    {
        auto result = m_swapChain->GetBuffer(i, IID_PPV_ARGS(m_renderTargets[i].GetAddressOf()));
        if (FAILED(result)) return false;

        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        rtvDesc.Texture2D.MipSlice = 0;
        rtvDesc.Texture2D.PlaneSlice = 0;

        device->CreateRenderTargetView(m_renderTargets[i].Get(), &rtvDesc, handle);
        handle.Offset(1, m_rtvDescriptorSize);
    }

    return true;
}

bool SwapChainPresenter::CreateDepthBuffer(Device& device)
{
    Assert(m_size.width > 0);
    Assert(m_size.height > 0);

    D3D12_RESOURCE_DESC desc =
        CD3DX12_RESOURCE_DESC::Tex2D(
            m_depthFormat,
            m_size.width,
            m_size.height,
            1,
            0,
            1,
            0,
            D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

    D3D12_CLEAR_VALUE clear{};
    clear.Format = m_depthFormat;
    clear.DepthStencil.Depth = 1.0f;
    clear.DepthStencil.Stencil = 0;

    m_depthBuffer = device.CreateResource(
        desc,
        D3D12_HEAP_TYPE_DEFAULT,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clear);
    
    m_dsvHeap = device.CreateDescriptorHeap(
        D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
        1,
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

    auto dsvHandle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = m_depthFormat;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;

    device->CreateDepthStencilView(m_depthBuffer.Get(), &dsvDesc, dsvHandle);

    return true;
}