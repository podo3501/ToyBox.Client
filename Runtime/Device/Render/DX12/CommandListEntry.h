#pragma once
#include <wrl/client.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;

class CommandListEntry
{
public:
    ~CommandListEntry();
    CommandListEntry();
    bool Initialize(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
    void Reset();
    void Close();
    bool IsAvailable() const;
    void SetFence(ID3D12Fence* fence, uint64_t value);
    ID3D12GraphicsCommandList* Get() { return m_command.Get(); }

private:
    ComPtr<ID3D12CommandAllocator> m_allocator;
    ComPtr<ID3D12GraphicsCommandList> m_command;

    ID3D12Fence* m_fence = nullptr;
    uint64_t m_lastFenceValue = 0;
};