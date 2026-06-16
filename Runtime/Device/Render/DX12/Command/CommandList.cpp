#include "pch.h"
#include "CommandList.h"
#include "Core/Device.h"
#include "Core/D3D12Conversions.h"

struct QueueDependency
{
    CommandType type;
    uint64_t fenceValue;
};

CommandList::~CommandList() = default;
CommandList::CommandList() = default;

bool CommandList::Initialize(Device& device, CommandType type)
{
    ReturnIfFailed(device->CreateCommandAllocator(ToD3D12(type), IID_PPV_ARGS(&m_allocator)));
    ReturnIfFailed(device->CreateCommandList(0, ToD3D12(type), m_allocator.Get(), nullptr, IID_PPV_ARGS(&m_command)));
    Close(); // 초기 상태는 닫아둠

    m_type = type;
    return true;
}

void CommandList::Reset()
{
    Assert(!m_recording);

    if (m_lastFenceValue != 0)
    {
        Assert(m_fence);
        Assert(m_fence->GetCompletedValue() >= m_lastFenceValue); //이전 gpu 작업이 끝났는지 확인.
    }

    DxCheck(m_allocator->Reset());
    DxCheck(m_command->Reset(m_allocator.Get(), nullptr));

    m_recording = true;
}

void CommandList::Close()
{
    DxCheck(m_command->Close());
}

void CommandList::SetBindlessHeap(ID3D12DescriptorHeap* heap)
{
    Assert(heap);

    ID3D12DescriptorHeap* heaps[] = { heap };
    m_command->SetDescriptorHeaps(1, heaps);
}

void CommandList::SetFence(ID3D12Fence* fence, uint64_t value)
{
    Assert(value != 0);

    m_fence = fence;
    m_lastFenceValue = value;

    m_recording = false;
}

bool CommandList::IsAvailable() const
{
    if (m_recording)
        return false;

    if (m_lastFenceValue == 0) //한번도 쓴적이 없다면
        return true;

    Assert(m_fence);

    bool completed = m_fence->GetCompletedValue() >= m_lastFenceValue; //시킨 일이 끝나 있는지
    return completed;
}

void CommandList::DependOn(CommandType type, uint64_t fenceValue)
{
    m_dependencies.push_back({ type, fenceValue });
}

const std::vector<QueueDependency>& CommandList::GetDependencies() const
{
    return m_dependencies;
}