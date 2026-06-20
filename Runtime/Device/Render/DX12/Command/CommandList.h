#pragma once
#include <wrl/client.h>
#include <d3d12.h>
#include "CommandType.h"

struct QueueDependency
{
    CommandType type;
    uint64_t fenceValue;
};

class Device;

using Microsoft::WRL::ComPtr;

class CommandList
{
public:
    ~CommandList();
    CommandList();

    CommandList(const CommandList&) = delete;
    CommandList& operator=(const CommandList&) = delete;
    CommandList(CommandList&&) noexcept = default;
    CommandList& operator=(CommandList&&) noexcept = default;

    bool Initialize(Device& device, CommandType type);
    void Reset();
    void Close();

    bool IsAvailable() const;
    void SetBindlessHeap(ID3D12DescriptorHeap* heap);
    void MarkSubmitted(ID3D12Fence* fence, uint64_t value);

    ID3D12GraphicsCommandList* operator->() const { return m_command.Get(); }
    ID3D12GraphicsCommandList* Get() { return m_command.Get(); }
    uint64_t GetFence() const noexcept { return m_lastFenceValue; }
    CommandType GetType() const noexcept { return m_type; }

private:
    CommandType m_type{ CommandType::None };
    bool m_recording{ false }; //cpu가 명령어를 쓰는 중.

    ComPtr<ID3D12CommandAllocator> m_allocator;
    ComPtr<ID3D12GraphicsCommandList> m_command;

    ID3D12Fence* m_fence{ nullptr }; //gpu가 쓰는중
    uint64_t m_lastFenceValue{ 0 }; // 0값은 초기값이다. 사용하지 않음.
};