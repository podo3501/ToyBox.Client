#pragma once
#include <wrl/client.h>
#include "DX12DeviceView.h"

struct ID3D12CommandAllocator;
struct ID3D12GraphicsCommandList;
struct ID3D12Fence;

class CommandScheduler
{
public:
    ~CommandScheduler();
    CommandScheduler(const DX12DeviceView& dv);
    bool Initialize();
    bool BeginFrame();
    bool EndFrame();
    bool FlushGPU();

    ID3D12GraphicsCommandList* GetCommandList();

private:
    bool CreateCommandObjects();
    bool CreateFence();

    DX12DeviceView m_dv{};
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_allocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
    Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValue{ 0 };
    HANDLE m_eventHandle{ nullptr };
};
