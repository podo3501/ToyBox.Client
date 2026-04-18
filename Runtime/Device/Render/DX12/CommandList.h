#pragma once
#include <wrl/client.h>
#include <d3d12.h>
#include "CommandType.h"

class DescriptorAllocation;

using Microsoft::WRL::ComPtr;

class CommandList
{
public:
    ~CommandList();
    CommandList();
    bool Initialize(ID3D12Device* device, CommandType type);
    void Reset();
    void Close();

    void EnqueueDeferredDescriptors(DescriptorAllocation&& descriptor);
    bool IsAvailable() const;
    void SetFence(ID3D12Fence* fence, uint64_t value);

    ID3D12GraphicsCommandList* operator->() const { return m_command.Get(); }
    ID3D12GraphicsCommandList* Get() { return m_command.Get(); }

private:
    ComPtr<ID3D12CommandAllocator> m_allocator;
    ComPtr<ID3D12GraphicsCommandList> m_command;

    CommandType m_type;
    ID3D12Fence* m_fence{ nullptr };
    uint64_t m_lastFenceValue{ 0 };
    vector<DescriptorAllocation> m_deferredDescriptors;
};