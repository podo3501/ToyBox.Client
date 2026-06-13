#include "pch.h"
#include "CommandQueue.h"
#include "CommandList.h"
#include "Core/Device.h"

CommandQueue::~CommandQueue() 
{ 
    if (m_event)
    {
        CloseHandle(m_event);
        m_event = nullptr;
    }
}
CommandQueue::CommandQueue() = default;

bool CommandQueue::Initialize(Device& device, CommandType type, uint32_t cmdPoolSize)
{
    if (cmdPoolSize <= 0) return false;

    ReturnIfFalse(CreateQueue(device, type));
    ReturnIfFalse(CreateFence(device));

    m_pool.reserve(cmdPoolSize);
    for (uint32_t i = 0; i < cmdPoolSize; ++i)
    {
        auto context = make_unique<CommandList>();
        ReturnIfFalse(context->Initialize(device, type));

        m_pool.emplace_back(move(context));
    }

    return true;
}

CommandList* CommandQueue::Begin()
{
    Assert(!m_currentCmdEntry);

    auto entry = GetAvailableCommandList();
    if (!entry) return nullptr;

    entry->Reset();
    m_currentCmdEntry = entry;

    return entry;
}

uint64_t CommandQueue::End()
{
    Assert(m_currentCmdEntry);

    m_currentCmdEntry->Close();

    ID3D12CommandList* lists[] = { m_currentCmdEntry->Get() };
    m_queue->ExecuteCommandLists(1, lists);

    uint64_t fenceValue = Signal();
    m_lastSubmittedFence = fenceValue;

    m_currentCmdEntry->SetFence(m_fence.Get(), fenceValue); // 재사용하기 위해서 fence 기록

    m_currentCmdEntry = nullptr;
    return fenceValue;
}

uint64_t CommandQueue::Signal()
{
    uint64_t value = ++m_fenceValue;
    DxCheck(m_queue->Signal(m_fence.Get(), value));
    return value;
}

void CommandQueue::WaitIdle()
{
    if (m_fenceValue > 1)
        WaitFence(m_fenceValue - 1);
}

bool CommandQueue::CreateQueue(Device& device, CommandType type)
{
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = ToD3D12(type);
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    return SUCCEEDED(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_queue)));
}

bool CommandQueue::CreateFence(Device& device)
{
    if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence))))
        return false;

    m_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    return m_event != nullptr;
}

CommandList* CommandQueue::GetAvailableCommandList()
{
    for (size_t i = 0; i < m_pool.size(); ++i)
    {
        CommandList* entry = m_pool[m_next].get();
        m_next = (m_next + 1) % m_pool.size();

        if (entry->IsAvailable())
            return entry;
    }

    return nullptr; // 사용 가능한 context 없음 여기서 만약 while로 기다리게 되면 cpu, gpu 동기화가 되기 때문에 일부러 nullptr 리턴함. begin에서 nullptr이면 present 안하고 리턴. 의도한 바임.
}

void CommandQueue::WaitFence(uint64_t value)
{
    if (m_fence->GetCompletedValue() < value)
    {
        m_fence->SetEventOnCompletion(value, m_event);
        WaitForSingleObject(m_event, INFINITE);
    }
}