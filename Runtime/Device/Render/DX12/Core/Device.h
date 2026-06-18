#pragma once
#include "Resource/Resource.h"
#include <wrl/client.h>
#include <d3dcommon.h>

struct ID3D12Device;
struct IDXGIFactory4;
struct ID3D12CommandQueue;

class Device
{
public:
    ~Device();
    explicit Device(bool enableDebug);
    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    Resource CreateResource(
        const D3D12_RESOURCE_DESC& desc,
        D3D12_HEAP_TYPE heapType,
        D3D12_RESOURCE_STATES state,
        const D3D12_CLEAR_VALUE* clearValue = nullptr);

    ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(
        D3D12_DESCRIPTOR_HEAP_TYPE type,
        UINT numDescriptors,
        D3D12_DESCRIPTOR_HEAP_FLAGS flags);

    ComPtr<ID3D12QueryHeap> CreateQueryHeap(D3D12_QUERY_HEAP_TYPE type, UINT count);
    ComPtr<ID3D12Fence> CreateFence(UINT64 initialValue, D3D12_FENCE_FLAGS flags);

    UINT64 GetRequiredIntermediateSize(
        const D3D12_RESOURCE_DESC& desc,
        UINT firstSubresource,
        UINT numSubresources,
        UINT64 offset);

    ID3D12Device* operator->() const { return m_device.Get(); }
    ID3D12Device* Get() { return m_device.Get(); }
    IDXGIFactory4* GetFactory() const { return m_dxgiFactory.Get(); }
    D3D_FEATURE_LEVEL GetFeatureLevel() const { return m_featureLevel; }

private:
    float a[10000000];

    bool CreateFactory(bool enableDebug);
    bool CreateDevice();
    
    bool TryCreateDevice(IUnknown* adapter);

    Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
    Microsoft::WRL::ComPtr<ID3D12Device> m_device;

    D3D_FEATURE_LEVEL m_featureLevel{ D3D_FEATURE_LEVEL_11_0 };
};