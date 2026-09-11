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

void CommandList::SetBindlessHeap(ID3D12DescriptorHeap* heap)
{
    Assert(heap);

    ID3D12DescriptorHeap* heaps[] = { heap };
    m_command->SetDescriptorHeaps(1, heaps);
}

void CommandList::SetGraphicsRootSignature(ID3D12RootSignature* rootSignature)
{
    Assert(rootSignature);

    if (m_currentRootSignature == rootSignature)
        return;

    m_command->SetGraphicsRootSignature(rootSignature);
    m_currentRootSignature = rootSignature;
}

void CommandList::SetPipelineState(
    ID3D12PipelineState* pso,
    std::optional<PrimitiveTopologyType> topology)
{
    Assert(pso);

    if (m_currentPSO != pso)
    {
        m_command->SetPipelineState(pso);
        m_currentPSO = pso;
    }

    if (topology)
        m_command->IASetPrimitiveTopology(ToD3D12_Draw(*topology));
}

void CommandList::Reset()
{
    Assert(m_state == CmdState::Ready);

    if (m_lastFenceID != 0)
    {
        Assert(m_fence);
        Assert(m_fence->GetCompletedValue() >= m_lastFenceID); //이전 gpu 작업이 끝났는지 확인.
    }

    DxCheck(m_allocator->Reset());
    DxCheck(m_command->Reset(m_allocator.Get(), nullptr));
    m_currentRootSignature = nullptr;
    m_currentPSO = nullptr;

    m_state = CmdState::Recording;
}

void CommandList::Close()
{
    Assert(m_state == CmdState::Recording);
    DxCheck(m_command->Close());

    m_state = CmdState::PendingSubmit;
}

bool CommandList::IsAvailable() const
{
    switch (m_state)
    {
    case CmdState::Ready:
        return true;

    case CmdState::InFlight:
        Assert(m_fence);
        if (m_fence->GetCompletedValue() >= m_lastFenceID) //시킨 일이 끝나 있는지
        {
            m_state = CmdState::Ready; // fence 완료 확인되면 여기서 상태를 확정적으로 되돌림
            return true;
        }
        return false;

    case CmdState::Recording:
    case CmdState::PendingSubmit:
    default:
        return false;
    }
}

void CommandList::MarkSubmitted(ID3D12Fence* fence, FenceID fenceID)
{
    Assert(m_state == CmdState::PendingSubmit); // Close 없이 호출되는 실수 경로 차단
    Assert(fence);
    Assert(fenceID != 0);

    m_fence = fence;
    m_lastFenceID = fenceID;
    m_state = CmdState::InFlight;
}

void CommandList::Discard()
{
    Assert(m_state == CmdState::Recording || m_state == CmdState::PendingSubmit);

    m_state = CmdState::Ready;
}