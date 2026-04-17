#include "pch.h"
#include "CommandList.h"
#include "DescriptorAllocation.h"

CommandList::~CommandList() = default;
CommandList::CommandList() = default;

bool CommandList::Initialize(ID3D12Device* device, CommandType type)
{
    ReturnIfFailed(device->CreateCommandAllocator(ToD3D12(type), IID_PPV_ARGS(&m_allocator)));
    ReturnIfFailed(device->CreateCommandList(0, ToD3D12(type), m_allocator.Get(), nullptr, IID_PPV_ARGS(&m_command)));
    Close(); // 초기 상태는 닫아둠

    m_type = type;
    return true;
}

void CommandList::Reset()
{
    DxCheck(m_allocator->Reset());
    DxCheck(m_command->Reset(m_allocator.Get(), nullptr));
}

void CommandList::Close()
{
    DxCheck(m_command->Close());
}

void CommandList::EnqueueDeferredDescriptors(DescriptorAllocation&& descriptor)
{
    m_deferredDescriptors.emplace_back(move(descriptor));
}

bool CommandList::IsAvailable() const
{
    if (!m_fence)
        return true; // 아직 한 번도 안 쓴 경우

    return m_fence->GetCompletedValue() >= m_lastFenceValue;
}

void CommandList::SetFence(ID3D12Fence* fence, uint64_t value)
{
    m_fence = fence;
    m_lastFenceValue = value;

    for (auto& desc : m_deferredDescriptors)
        desc.SetDeferredContext(m_type, value);
    m_deferredDescriptors.clear();
}