#include "pch.h"
#include "Device.h"
#include "DebugHelper.h"
#include "DebugOptions.h"
#include "d3dx12.h"
#include <dxgi1_6.h>

using Microsoft::WRL::ComPtr;

Device::~Device() = default;
Device::Device(bool enableDebug)
{
#if defined(_DEBUG)
    DebugOptions opt;
    opt.enableDebugLayer = true;
    opt.enableGpuValidation = false;   //필요할 때만 true. 계속 켜 놓으면 오래 걸린다.
    opt.breakOnWarning = false; // Warning도 잡는다.

    if (enableDebug)
        DebugHelper::EnableDebugLayer(opt);
#endif

    bool result = CreateFactory(enableDebug);
    Assert(result);

    result = CreateDevice();
    Assert(result);

#if defined(_DEBUG)
    DebugHelper::SetupInfoQueue(m_device.Get(), opt);
#endif
}

bool Device::CreateFactory(bool enableDebug)
{
    UINT flags = enableDebug ? DXGI_CREATE_FACTORY_DEBUG : 0;
    return SUCCEEDED(CreateDXGIFactory2(flags, IID_PPV_ARGS(&m_dxgiFactory)));
}

bool Device::TryCreateDevice(IUnknown* adapter)
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

bool Device::CreateDevice()
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