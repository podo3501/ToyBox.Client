#pragma once
#include <wrl/client.h>
#include <d3dcommon.h>

struct ID3D12Device;
struct IDXGIFactory4;
struct ID3D12CommandQueue;
struct DX12DeviceView;

class DX12Core
{
public:
    DX12Core() = default;
    ~DX12Core() = default;

    bool Initialize(bool enableDebug);

    ID3D12Device* GetDevice() const { return m_device.Get(); }
    IDXGIFactory4* GetFactory() const { return m_dxgiFactory.Get(); }

    DX12DeviceView GetDeviceView() const; //멤버변수들이 빈번하게 쓰이기 때문에 Get 함수를 두는 것보다 struct로 내보낸다.
    D3D_FEATURE_LEVEL GetFeatureLevel() const { return m_featureLevel; }

private:
    bool CreateFactory(bool enableDebug);
    bool CreateDevice();
    
    bool TryCreateDevice(IUnknown* adapter);

    Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
    Microsoft::WRL::ComPtr<ID3D12Device> m_device;

    D3D_FEATURE_LEVEL m_featureLevel{ D3D_FEATURE_LEVEL_11_0 };
};