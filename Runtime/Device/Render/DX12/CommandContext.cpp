#include "pch.h"
#include "CommandContext.h"

CommandContext::~CommandContext()
{
    if (m_eventHandle)
        CloseHandle(m_eventHandle);
}
CommandContext::CommandContext() = default;

bool CommandContext::Initialize(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type)
{
    //D3D12_COMMAND_QUEUE_DESC queueDesc{};
    //queueDesc.Type = type;
    //queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    
    //ReturnIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_queue)));
    ReturnIfFailed(device->CreateCommandAllocator(type, IID_PPV_ARGS(&m_allocator)));
    ReturnIfFailed(device->CreateCommandList(0, type, m_allocator.Get(), nullptr, IID_PPV_ARGS(&m_command)));
    m_command->Close(); // 초기 상태는 닫아둠

    ReturnIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
    m_fenceValue = 1;
    m_eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (!m_eventHandle) return false;

    return true;
}

bool CommandContext::Reset()
{
    ReturnIfFailed(m_allocator->Reset());
    ReturnIfFailed(m_command->Reset(m_allocator.Get(), nullptr));
    return true;
}

bool CommandContext::Close()
{
    //ReturnIfFailed(m_command->Close());

    //ID3D12CommandList* lists[] = { m_command.Get() };
    //m_queue->ExecuteCommandLists(1, lists);

    //return true;

    return SUCCEEDED(m_command->Close());
}

//uint64_t CommandContext::Signal()
//{
//    const uint64_t fenceValue = m_fenceValue;
//    ReturnIfFailed(m_queue->Signal(m_fence.Get(), fenceValue)); // GPU에게 "여기까지 오면 fenceValue 찍어라"라고 예약
//
//    m_fenceValue++;
//    return fenceValue;
//}

uint64_t CommandContext::NextFenceValue()
{
    return m_fenceValue++;
}

uint64_t CommandContext::GetCompletedValue() const
{
    return m_fence->GetCompletedValue();
}

bool CommandContext::WaitForFence(uint64_t fenceValue)
{
    if (m_fence->GetCompletedValue() >= fenceValue)
        return true;

    ReturnIfFailed(m_fence->SetEventOnCompletion(fenceValue, m_eventHandle));
    WaitForSingleObject(m_eventHandle, INFINITE);

    return true;
}

//
//bool CommandContext::Flush()
//{
//    if (!m_queue) return false;
//
//    const UINT64 fenceToWait = m_fenceValue;
//    ReturnIfFailed(m_queue->Signal(m_fence.Get(), fenceToWait));
//
//    m_fenceValue++;
//    if (m_fence->GetCompletedValue() < fenceToWait)
//    {
//        ReturnIfFailed(m_fence->SetEventOnCompletion(fenceToWait, m_eventHandle));
//        WaitForSingleObject(m_eventHandle, INFINITE);
//    }
//
//    return true;
//}