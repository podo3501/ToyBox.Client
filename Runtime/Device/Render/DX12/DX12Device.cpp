#include "pch.h"
#include "DX12Device.h"
#include "DebugHelper.h"
#include "DX12DeviceView.h"
#include "d3dx12.h"
#include <dxgi1_6.h>

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

bool DX12Device::TryCreateDevice(IUnknown* adapter)
{
    const D3D_FEATURE_LEVEL levels[] =
    {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1
    };

    for (auto level : levels)
    {
        if (SUCCEEDED(D3D12CreateDevice(adapter, level, IID_PPV_ARGS(&m_device))))
        {
            m_featureLevel = level;
            return true;
        }
    }

    return false;
}


bool DX12Device::CreateDevice()
{
    ComPtr<IDXGIAdapter1> adapter;

    for (UINT i = 0; m_dxgiFactory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
    {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

        if (TryCreateDevice(adapter.Get()))
            return true;

        adapter.Reset();
    }

    ComPtr<IDXGIAdapter> warpAdapter; // fallback
    if (FAILED(m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter))))
        return false;

    return TryCreateDevice(warpAdapter.Get());
}

bool DX12Device::CreateCommandQueue()
{
    D3D12_COMMAND_QUEUE_DESC desc{};
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    return SUCCEEDED(m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue)));
}

DX12DeviceView DX12Device::GetDeviceView() const
{
    return { m_device.Get(), m_commandQueue.Get(), m_dxgiFactory.Get() };
}