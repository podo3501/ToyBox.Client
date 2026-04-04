#pragma once
#include <wrl/client.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

class CommandContext
{
public:
    ~CommandContext();
    CommandContext();
    bool Initialize(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
    bool Reset();
    bool Close();
    //bool Flush(); // GPU 동기화
    
    //uint64_t Signal(); // GPU에 fence 신호만 보냄 (비동기)
    uint64_t NextFenceValue();
    uint64_t GetCompletedValue() const; // 현재 GPU 완료된 fence 값
    bool WaitForFence(uint64_t fenceValue); // 필요할 때만 쓰는 강제 동기화

    ID3D12GraphicsCommandList* Get() { return m_command.Get(); }
    ID3D12Fence* GetFence() { return m_fence.Get(); }
    //ID3D12CommandQueue* GetQueue() { return m_queue.Get(); }
    uint64_t GetLastSignaledFence() const { return m_fenceValue - 1; } // Signal()에서 증가되므로 -1

private:
    ComPtr<ID3D12CommandAllocator> m_allocator;
    ComPtr<ID3D12GraphicsCommandList> m_command;
    //ComPtr<ID3D12CommandQueue> m_queue;
    ComPtr<ID3D12Fence> m_fence;

    UINT64 m_fenceValue{ 0 };
    HANDLE m_eventHandle{ nullptr };
};