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
    m_type = type;

    D3D12_COMMAND_QUEUE_DESC queueDesc{};
    queueDesc.Type = m_type;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    
    ReturnIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_queue)));
    ReturnIfFailed(device->CreateCommandAllocator(m_type, IID_PPV_ARGS(&m_allocator)));
    ReturnIfFailed(device->CreateCommandList(0, m_type, m_allocator.Get(), nullptr, IID_PPV_ARGS(&m_command)));
    m_command->Close(); // ÃÊ±â »óÅÂ´Â ´Ý¾ÆµÒ

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
    ReturnIfFailed(m_command->Close());

    ID3D12CommandList* lists[] = { m_command.Get() };
    m_queue->ExecuteCommandLists(1, lists);

    return true;
}

bool CommandContext::Flush()
{
    if (!m_queue) return false;

    const UINT64 fenceToWait = m_fenceValue;
    ReturnIfFailed(m_queue->Signal(m_fence.Get(), fenceToWait));

    m_fenceValue++;
    if (m_fence->GetCompletedValue() < fenceToWait)
    {
        ReturnIfFailed(m_fence->SetEventOnCompletion(fenceToWait, m_eventHandle));
        WaitForSingleObject(m_eventHandle, INFINITE);
    }

    return true;
}