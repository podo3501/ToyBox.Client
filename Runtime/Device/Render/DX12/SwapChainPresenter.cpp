#include "pch.h"
#include "SwapChainPresenter.h"
#include "CommandScheduler.h"
#include <dxgi1_6.h>
#include "CommandList.h"
#include "CommandUtils.h"

using Microsoft::WRL::ComPtr;

SwapChainPresenter::~SwapChainPresenter()
{
    if (m_scheduler)
        m_scheduler->WaitQueueIdle(CommandType::Direct);
}
SwapChainPresenter::SwapChainPresenter() = default;

bool SwapChainPresenter::Initialize(ID3D12Device* device, IDXGIFactory4* factory,
    CommandScheduler* scheduler, const SwapChainDesc& desc)
{
    m_frameCount = desc.frameCount;
    m_renderTargets.resize(m_frameCount);

    auto queue = scheduler->GetCommandQueue(CommandType::Direct);
    ReturnIfFalse(CreateSwapChain(device, factory, queue, desc));
    ReturnIfFalse(CreateRTV(device));

    m_scheduler = scheduler;
    m_size = desc.size;
    m_tearing = desc.allowTearing;
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    return true;
}

void SwapChainPresenter::SetRenderTarget(CommandList& cmd)
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
        m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
        m_frameIndex,
        m_rtvDescriptorSize);
    CommandUtils::SetRenderTarget(cmd, rtvHandle);
    CommandUtils::SetViewport(cmd, static_cast<float>(m_size.width), static_cast<float>(m_size.height));
    CommandUtils::SetScissor(cmd, m_size.width, m_size.height);
}

void SwapChainPresenter::TransitionToRenderTarget(CommandList& cmd)
{
    CommandUtils::Transition(cmd, m_renderTargets[m_frameIndex].Get(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void SwapChainPresenter::TransitionToPresent(CommandList& cmd)
{
    CommandUtils::Transition(cmd, m_renderTargets[m_frameIndex].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);
}

bool SwapChainPresenter::Present(bool vsync)
{
    UINT syncInterval = vsync ? 1 : 0;
    UINT flags = (!vsync && m_tearing) ? DXGI_PRESENT_ALLOW_TEARING : 0;
    if (FAILED(m_swapChain->Present(syncInterval, flags))) return false;

    m_scheduler->SignalQueue(CommandType::Direct);
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
    return true;
}

bool SwapChainPresenter::Resize(ID3D12Device* device, const Size& size)
{
    if (size.width == 0 || size.height == 0) return false;
    if (m_size == size) return true;

    m_scheduler->WaitForAllGPU(); // GPU 작업 끝날 때까지 대기

    for (UINT i = 0; i < m_frameCount; ++i)
        m_renderTargets[i].Reset(); //기존 RTV 리소스 해제

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

    return CreateFrameRTVs(device);
}

D3D12_CPU_DESCRIPTOR_HANDLE SwapChainPresenter::GetCurrentRTV() const
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE handle(
        m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
    return handle;
}

bool SwapChainPresenter::CreateSwapChain(ID3D12Device* device, IDXGIFactory4* factory,
    ID3D12CommandQueue* queue, const SwapChainDesc& desc)
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

bool SwapChainPresenter::CreateRTV(ID3D12Device* device)
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
    heapDesc.NumDescriptors = m_frameCount;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    if (FAILED(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_rtvHeap))))
        return false;

    m_rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    
    return CreateFrameRTVs(device);
}

bool SwapChainPresenter::CreateFrameRTVs(ID3D12Device* device)
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

    for (UINT i = 0; i < m_frameCount; ++i)
    {
        if (FAILED(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])))) return false;

        device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, handle);
        handle.Offset(1, m_rtvDescriptorSize);
    }

    return true;
}