#pragma once
#include <wrl/client.h>

struct ID3D12Device;
struct IDXGIFactory4;
struct ID3D12CommandQueue;
struct DX12Core;

class DX12Device
{
public:
    DX12Device() = default;
    ~DX12Device() = default;

    bool Initialize(bool enableDebug);

    ID3D12Device* GetDevice() const { return m_device.Get(); }
    ID3D12CommandQueue* GetCommandQueue() const { return m_commandQueue.Get(); }

    DX12Core GetCore() const; //멤버변수들이 번번하게 쓰이기 때문에 Get 함수를 두는 것보다 struct로 내보낸다.

private:
    bool CreateFactory(bool enableDebug);
    bool CreateDevice();
    bool CreateCommandQueue();

    Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
    Microsoft::WRL::ComPtr<ID3D12Device> m_device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
};