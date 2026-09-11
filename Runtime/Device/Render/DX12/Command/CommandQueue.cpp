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

bool CommandQueue::Initialize(
    Device& device, 
    ID3D12DescriptorHeap* bindlessHeap,
    CommandType type, 
    uint32_t cmdPoolSize)
{
    if (cmdPoolSize <= 0) return false;

    ReturnIfFalse(CreateQueue(device, type));
    ReturnIfFalse(CreateFence(device));

    m_bindlessHeap = bindlessHeap;

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

    PrepareCommandList(*entry);
    m_currentCmdEntry = entry;

    return entry;
}

FenceID CommandQueue::End()
{
    Assert(m_currentCmdEntry);

    m_currentCmdEntry->Close();
    m_pendingSubmission.push_back(m_currentCmdEntry);
    m_currentCmdEntry = nullptr;

    std::vector<ID3D12CommandList*> raw;
    raw.reserve(m_pendingSubmission.size());
    for (auto* cmdList : m_pendingSubmission)
        raw.push_back(cmdList->Get());

    m_queue->ExecuteCommandLists(static_cast<UINT>(raw.size()), raw.data());

    FenceID fenceID = Signal();
    m_lastSubmittedFence = fenceID;

    for (auto* cmdList : m_pendingSubmission)
        cmdList->MarkSubmitted(m_fence.Get(), fenceID);

    m_pendingSubmission.clear();
    return fenceID;
}

std::vector<CommandList*> CommandQueue::BeginParallel(size_t count)
{
    Assert(m_currentCmdEntry); // Begin()으로 연 상태에서만 호출 가능

    m_currentCmdEntry->Close();
    m_pendingSubmission.push_back(m_currentCmdEntry);
    m_currentCmdEntry = nullptr;

    std::vector<CommandList*> result;
    result.reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
        CommandList* entry = GetAvailableCommandList();
        if (!entry) break;

        PrepareCommandList(*entry);
        result.push_back(entry);
    }
    return result;
}

CommandList* CommandQueue::EndParallel(std::span<CommandList*> cmdLists)
{
    Assert(!cmdLists.empty());

    for (auto* entry : cmdLists)
    {
        entry->Close();
        m_pendingSubmission.push_back(entry);
    }

    auto entry = GetAvailableCommandList();
    if (!entry) return nullptr; // pool 고갈 - 이전에 짚은 이슈 여전히 남음

    PrepareCommandList(*entry);
    m_currentCmdEntry = entry;

    return entry;
}

void CommandQueue::PrepareCommandList(CommandList& cmd)
{
    cmd.Reset();

    if (m_bindlessHeap)
        cmd.SetBindlessHeap(m_bindlessHeap);
}

void CommandQueue::AbortFrame()
{
    // 기록 중이던 primary Discard
    if (m_currentCmdEntry)
    {
        m_currentCmdEntry->Discard();
        m_currentCmdEntry = nullptr;
    }

    for (auto* entry : m_pendingSubmission)
        entry->Discard(); // Close는 됐지만 아직 ExecuteCommandLists를 안 탄 것들 전부 폐기

    m_pendingSubmission.clear();
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