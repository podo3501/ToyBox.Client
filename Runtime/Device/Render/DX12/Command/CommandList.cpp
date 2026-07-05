#include "pch.h"
#include "CommandList.h"
#include "Core/Device.h"
#include "Core/D3D12Conversions.h"

CommandList::~CommandList() = default;
CommandList::CommandList() = default;

bool CommandList::Initialize(Device& device, CommandType type)
{
    ReturnIfFailed(device->CreateCommandAllocator(ToD3D12(type), IID_PPV_ARGS(&m_allocator)));
    ReturnIfFailed(device->CreateCommandList(0, ToD3D12(type), m_allocator.Get(), nullptr, IID_PPV_ARGS(&m_command)));
    m_command->Close(); // 초기 상태는 닫아둠

    m_type = type;
    return true;
}

void CommandList::Reset()
{
    Assert(!m_recording);

    if (m_lastFenceID != 0)
    {
        Assert(m_fence);
        Assert(m_fence->GetCompletedValue() >= m_lastFenceID); //이전 gpu 작업이 끝났는지 확인.
    }

    DxCheck(m_allocator->Reset());
    DxCheck(m_command->Reset(m_allocator.Get(), nullptr));

    m_recording = true;
}

void CommandList::Close()
{
    Assert(m_recording);
    DxCheck(m_command->Close());

    m_recording = false;
}

void CommandList::SetBindlessHeap(ID3D12DescriptorHeap* heap)
{
    Assert(heap);

    ID3D12DescriptorHeap* heaps[] = { heap };
    m_command->SetDescriptorHeaps(1, heaps);
}

void CommandList::MarkSubmitted(ID3D12Fence* fence, FenceID fenceID)
{
    Assert(!m_recording);
    Assert(fence);
    Assert(fenceID != 0);

    m_fence = fence;
    m_lastFenceID = fenceID;
}

bool CommandList::IsAvailable() const
{
    if (m_recording)
        return false;

    if (m_lastFenceID == 0) //한번도 쓴적이 없다면
        return true;

    Assert(m_fence);

    bool completed = m_fence->GetCompletedValue() >= m_lastFenceID; //시킨 일이 끝나 있는지
    return completed;
}