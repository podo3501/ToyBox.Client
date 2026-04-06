#include "pch.h"
#include "CommandListEntry.h"

CommandListEntry::~CommandListEntry() = default;
CommandListEntry::CommandListEntry() = default;

bool CommandListEntry::Initialize(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type)
{
    ReturnIfFailed(device->CreateCommandAllocator(type, IID_PPV_ARGS(&m_allocator)));
    ReturnIfFailed(device->CreateCommandList(0, type, m_allocator.Get(), nullptr, IID_PPV_ARGS(&m_command)));
    Close(); // 초기 상태는 닫아둠

    return true;
}

void CommandListEntry::Reset()
{
    DxCheck(m_allocator->Reset());
    DxCheck(m_command->Reset(m_allocator.Get(), nullptr));
}

void CommandListEntry::Close()
{
    DxCheck(m_command->Close());
}

bool CommandListEntry::IsAvailable() const
{
    if (!m_fence)
        return true; // 아직 한 번도 안 쓴 경우

    return m_fence->GetCompletedValue() >= m_lastFenceValue;
}

void CommandListEntry::SetFence(ID3D12Fence* fence, uint64_t value)
{
    m_fence = fence;
    m_lastFenceValue = value;
}