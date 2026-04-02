#pragma once
#include <wrl/client.h>
#include <d3d12.h>

class CommandContext
{
public:
    ~CommandContext();
    CommandContext();
    bool Initialize(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
    bool Reset();
    bool Close();
    bool Flush(); // GPU µø±‚»≠

    ID3D12GraphicsCommandList* Get() { return m_command.Get(); }
    ID3D12CommandQueue* GetQueue() { return m_queue.Get(); }

private:
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_allocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_command;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_queue;
    Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;

    UINT64 m_fenceValue{ 0 };
    HANDLE m_eventHandle{ nullptr };
    D3D12_COMMAND_LIST_TYPE m_type{ D3D12_COMMAND_LIST_TYPE_DIRECT };
};