#include "pch.h"
#include "FrameDisplay.h"
#include "CommandScheduler.h"
#include <dxgi1_6.h>
#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

FrameDisplay::~FrameDisplay() = default;
FrameDisplay::FrameDisplay(DX12Core core) :
    m_core{ core }
{}

bool FrameDisplay::Initialize(HWND hwnd, const Size& size, bool allowTearing)
{
    ReturnIfFalse(CreateSwapChain(hwnd, size, allowTearing));
    ReturnIfFalse(CreateRTV());

    m_size = size;
    m_tearing = allowTearing;
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    return true;
}

void FrameDisplay::BindCurrentRTV(ID3D12GraphicsCommandList* cmdList)
{
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_renderTargets[m_frameIndex].Get(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET); // Resource Barrier: PRESENT -> RENDER_TARGET
    cmdList->ResourceBarrier(1, &barrier);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
        m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
        m_frameIndex,
        m_rtvDescriptorSize);
    cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    D3D12_VIEWPORT viewport{};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(m_size.width);
    viewport.Height = static_cast<float>(m_size.height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    cmdList->RSSetViewports(1, &viewport);

    D3D12_RECT scissor{};
    scissor.left = 0;
    scissor.top = 0;
    scissor.right = m_size.width;
    scissor.bottom = m_size.height;
    cmdList->RSSetScissorRects(1, &scissor);
}

bool FrameDisplay::Present(bool vsync)
{
    UINT syncInterval = vsync ? 1 : 0;
    UINT flags = (!vsync && m_tearing) ? DXGI_PRESENT_ALLOW_TEARING : 0;
    if (FAILED(m_swapChain->Present(syncInterval, flags))) return false;

    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
    return true;
}

bool FrameDisplay::Resize(CommandScheduler* cmd, const Size& size)
{
    if (size.width == 0 || size.height == 0) return false;
    if (m_size == size) return true;

    ReturnIfFalse(cmd->FlushGPU()); // GPU 작업 끝날 때까지 대기

    for (UINT i = 0; i < FrameCount; ++i)
        m_renderTargets[i].Reset(); //기존 RTV 리소스 해제

    DXGI_SWAP_CHAIN_DESC desc{}; //SwapChain Resize
    if (FAILED(m_swapChain->GetDesc(&desc)))
        return false;

    UINT flags = desc.Flags;

    if (FAILED(m_swapChain->ResizeBuffers(
        FrameCount,
        size.width,
        size.height,
        desc.BufferDesc.Format,
        flags)))
        return false;

    m_size = size;
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    return CreateFrameRTVs();
}

D3D12_CPU_DESCRIPTOR_HANDLE FrameDisplay::GetCurrentRTV() const
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE handle(
        m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
    return handle;
}

bool FrameDisplay::CreateSwapChain(HWND hwnd, const Size& wndSize, bool allowTearing)
{
    DXGI_SWAP_CHAIN_DESC1 scDesc{};
    scDesc.BufferCount = 2;
    scDesc.Width = wndSize.width;
    scDesc.Height = wndSize.height;
    scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scDesc.SampleDesc.Count = 1;
    scDesc.Flags = allowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

    ComPtr<IDXGISwapChain1> swapChain1;
    if (FAILED(m_core.factory->CreateSwapChainForHwnd(
        m_core.queue,
        hwnd,
        &scDesc,
        nullptr,
        nullptr,
        &swapChain1)))
        return false;

    m_core.factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

    return SUCCEEDED(swapChain1.As(&m_swapChain));
}

bool FrameDisplay::CreateRTV()
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
    heapDesc.NumDescriptors = FrameCount;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    if (FAILED(m_core.device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_rtvHeap))))
        return false;

    m_rtvDescriptorSize = m_core.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    
    return CreateFrameRTVs();
}

bool FrameDisplay::CreateFrameRTVs()
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

    for (UINT i = 0; i < FrameCount; ++i)
    {
        if (FAILED(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])))) return false;

        m_core.device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, handle);
        handle.Offset(1, m_rtvDescriptorSize);
    }

    return true;
}