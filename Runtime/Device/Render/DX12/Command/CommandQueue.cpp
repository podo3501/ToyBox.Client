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

    //---- 테스트용 인위적 GPU 지연 fence ----
    m_delayFence = device.CreateFence(0, D3D12_FENCE_FLAG_NONE);
    ReturnIfFalse(m_delayFence != nullptr);
    //-------------------------------------------

    m_bindlessHeap = bindlessHeap;

    m_pool.resize(cmdPoolSize);
    for (auto& cmd : m_pool)
        ReturnIfFalse(cmd.Initialize(device, type));

    return true;
}

CommandList* CommandQueue::Begin(UINT slot)
{
    Assert(slot < FrameBufferCount);

    // 0이면 아직 이 슬롯에 제출한 적이 없다는 뜻이므로 대기 없이 통과.
    // (Signal()이 ++m_fenceID로 1부터 시작하므로 0은 "미사용"을 의미)
    if (m_frameFences[slot] != 0)
        WaitFence(m_frameFences[slot]);

    CommandList* cmd = Begin();
    if (cmd)
        m_pendingFrameSlot = slot;

    return cmd;
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

    if (m_artificialDelayMs > 0)
        InjectArtificialGpuDelay(m_artificialDelayMs);

    m_queue->ExecuteCommandLists(static_cast<UINT>(raw.size()), raw.data());

    FenceID fenceID = Signal();
    m_lastSubmittedFence = fenceID;

    if (m_pendingFrameSlot != UINT_MAX)
    {
        m_frameFences[m_pendingFrameSlot] = fenceID;
        m_pendingFrameSlot = UINT_MAX;
    }

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
    m_pendingFrameSlot = UINT_MAX;
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

void CommandQueue::InjectArtificialGpuDelay(uint32_t delayMs)
{
    ++m_delayFenceValue;
    UINT64 targetValue = m_delayFenceValue;

    // GPU 타임라인에 "이 값이 signal 될 때까지 대기"를 꽂는다.
    // 이후 ExecuteCommandLists로 제출되는 커맨드는 이 대기가 풀려야 실행됨.
    DxCheck(m_queue->Wait(m_delayFence.Get(), targetValue));

    // CPU 스레드에서 delayMs 만큼 자다가 signal.
    // 테스트 전용이므로 detach thread로 단순하게 처리.
    ComPtr<ID3D12Fence> fence = m_delayFence; // 수명 보장을 위해 캡처
    std::thread([fence, targetValue, delayMs]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        fence->Signal(targetValue);
        }).detach();
}