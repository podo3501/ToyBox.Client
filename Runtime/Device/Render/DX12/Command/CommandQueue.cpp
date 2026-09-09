#include "pch.h"
#include "CommandQueue.h"
#include "CommandList.h"
#include "Core/Device.h"
#include "Core/D3D12Conversions.h"

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

    m_pool.resize(cmdPoolSize);
    for (auto& cmd : m_pool)
        ReturnIfFalse(cmd.Initialize(device, type));

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

FenceID CommandQueue::End()
{
    Assert(m_currentCmdEntry);

    m_currentCmdEntry->Close();

    ID3D12CommandList* lists[] = { m_currentCmdEntry->Get() };
    m_queue->ExecuteCommandLists(1, lists);

    FenceID fenceID = Signal();
    m_lastSubmittedFence = fenceID;

    m_currentCmdEntry->MarkSubmitted(m_fence.Get(), fenceID); // 재사용하기 위해서 fence 기록

    m_currentCmdEntry = nullptr;
    return fenceID;
}

std::vector<CommandList*> CommandQueue::BeginParallel(size_t count)
{
    // 중요: 이 함수는 반드시 메인 스레드에서만 호출되어야 함.
    Assert(!m_currentCmdEntry); // 기존 단일 Begin()과 동시 사용 금지

    std::vector<CommandList*> result;
    result.reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
        CommandList* entry = GetAvailableCommandList();
        if (!entry) break; // pool 부족하면 확보된 만큼만 반환 (Begin()의 nullptr 리턴과 동일한 의도)

        entry->Reset();
        result.push_back(entry);
    }

    return result;
}

FenceID CommandQueue::EndParallel(const std::vector<CommandList*>& cmdLists)
{
    std::vector<ID3D12CommandList*> raw;
    raw.reserve(cmdLists.size());
    for (auto* entry : cmdLists)
    {
        entry->Close(); // 여기서 통일해서 처리
        raw.push_back(entry->Get());
    }

    if (!raw.empty())
        m_queue->ExecuteCommandLists(static_cast<UINT>(raw.size()), raw.data());

    FenceID fenceID = Signal();
    m_lastSubmittedFence = fenceID;

    for (auto* entry : cmdLists)
        entry->MarkSubmitted(m_fence.Get(), fenceID);

    return fenceID;
}

FenceID CommandQueue::Signal()
{
    FenceID id = ++m_fenceID;
    DxCheck(m_queue->Signal(m_fence.Get(), id));
    return id;
}

void CommandQueue::WaitIdle()
{
    if (m_fenceID > 1)
        WaitFence(m_fenceID - 1);
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
    m_fence = device.CreateFence(0, D3D12_FENCE_FLAG_NONE);

    m_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    return m_event != nullptr;
}

CommandList* CommandQueue::GetAvailableCommandList()
{
    for (size_t i = 0; i < m_pool.size(); ++i)
    {
        CommandList* entry = &m_pool[m_next];
        m_next = (m_next + 1) % m_pool.size();

        if (entry->IsAvailable())
            return entry;
    }

    return nullptr; // 사용 가능한 context 없음 여기서 만약 while로 기다리게 되면 cpu, gpu 동기화가 되기 때문에 일부러 nullptr 리턴함. begin에서 nullptr이면 present 안하고 리턴. 의도한 바임.
}

void CommandQueue::WaitFence(FenceID fenceID)
{
    if (m_fence->GetCompletedValue() < fenceID)
    {
        m_fence->SetEventOnCompletion(fenceID, m_event);
        WaitForSingleObject(m_event, INFINITE);
    }
}