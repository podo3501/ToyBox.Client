#include "pch.h"
#include "DX12Device.h"
#include "Common.h"
#include "DebugHelper.h"
#include "DX12Core.h"

using Microsoft::WRL::ComPtr;

bool DX12Device::Initialize(bool enableDebug)
{
#if defined(_DEBUG)
    if (enableDebug)
        DebugHelper::EnableDebugLayer();
#endif

    ReturnIfFalse(CreateFactory(enableDebug));
    ReturnIfFalse(CreateDevice());
    ReturnIfFalse(CreateCommandQueue());

    return true;
}

bool DX12Device::CreateFactory(bool enableDebug)
{
    UINT flags = enableDebug ? DXGI_CREATE_FACTORY_DEBUG : 0;
    return SUCCEEDED(CreateDXGIFactory2(flags, IID_PPV_ARGS(&m_dxgiFactory)));
}

bool DX12Device::CreateDevice()
{
    ComPtr<IDXGIAdapter1> adapter;

    for (UINT i = 0; m_dxgiFactory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device), nullptr)))
            break;

        adapter.Reset();
    }

    if (!adapter)
    {
        ComPtr<IDXGIAdapter> warpAdapter;
        if (FAILED(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)))) return false;

        return SUCCEEDED(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_device)));
    }

    return SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_device)));
}

bool DX12Device::CreateCommandQueue()
{
    D3D12_COMMAND_QUEUE_DESC desc{};
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    return SUCCEEDED(m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue)));
}

DX12Core DX12Device::GetCore() const
{
    return { m_device.Get(), m_commandQueue.Get(), m_dxgiFactory.Get() };
}