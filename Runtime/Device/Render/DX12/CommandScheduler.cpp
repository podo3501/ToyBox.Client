#include "pch.h"
#include "CommandScheduler.h"
#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

CommandScheduler::~CommandScheduler()
{
    CloseHandleSafe(m_fenceDirectEvent);
    CloseHandleSafe(m_fenceCopyEvent);
}
CommandScheduler::CommandScheduler() = default;

bool CommandScheduler::Initialize(ID3D12Device* device, int poolSize)
{
    if (!CreateQueue(device, D3D12_COMMAND_LIST_TYPE_DIRECT, m_directQueue) ||
        !CreateQueue(device, D3D12_COMMAND_LIST_TYPE_COPY, m_copyQueue))
        return false;

    if (!CreateFence(device, m_directFence, m_fenceDirectEvent) ||
        !CreateFence(device, m_copyFence, m_fenceCopyEvent))
        return false;

    for (int i = 0; i < poolSize; ++i)
    {
        auto directCtx = make_unique<CommandContext>();
        ReturnIfFalse(directCtx->Initialize(device, D3D12_COMMAND_LIST_TYPE_DIRECT));
        m_directPool.emplace_back(move(directCtx));

        auto copyCtx = make_unique<CommandContext>();
        ReturnIfFalse(copyCtx->Initialize(device, D3D12_COMMAND_LIST_TYPE_COPY));
        m_copyPool.emplace_back(move(copyCtx));
    }
    return true;
}

CommandContext* CommandScheduler::GetAvailableContext(CommandType type)
{
    auto& pool = (type == CommandType::Direct) ? m_directPool : m_copyPool;
    size_t& nextIndex = (type == CommandType::Direct) ? m_nextDirect : m_nextCopy;

    for (size_t i = 0; i < pool.size(); ++i)
    {
        CommandContext* ctx = pool[nextIndex].get();
        nextIndex = (nextIndex + 1) % pool.size();

        if (ctx->IsAvailable())
            return ctx;
    }
    return nullptr; // 사용 가능한 context 없음
}

ID3D12GraphicsCommandList* CommandScheduler::Begin(CommandType type)
{
    Assert(!m_currentContext);

    auto ctx = GetAvailableContext(type);
    if (!ctx) return nullptr;

    ctx->Reset();
    m_currentContext = ctx;
    m_currentType = type;

    return ctx->Get();
}

uint64_t CommandScheduler::End(std::vector<ComPtr<ID3D12Resource>>&& resources)
{
    Assert(m_currentContext);

    m_currentContext->Close();

    ID3D12CommandQueue* queue = GetCommandQueue(m_currentType);
    ID3D12CommandList* lists[] = { m_currentContext->Get() };
    queue->ExecuteCommandLists(1, lists);

    uint64_t fenceValue = SignalFence(queue, m_currentType);

    auto fence = GetFence(m_currentType);
    m_currentContext->SetFence(fence, fenceValue); // context에 fence 기록 (allocator 재사용용)
    if (!resources.empty())
        m_pendingReleases.push({ fence, fenceValue, std::move(resources) });

    m_currentContext = nullptr;
    return fenceValue;
}

uint64_t CommandScheduler::SignalQueue(CommandType type)
{
    return SignalFence(GetCommandQueue(type), type);
}

uint64_t CommandScheduler::SignalFence(ID3D12CommandQueue* queue, CommandType type)
{
    ID3D12Fence* fence = GetFence(type);
    uint64_t value = IncrementFenceValue(type);

    DxCheck(queue->Signal(fence, value));
    return value;
}

ID3D12Fence* CommandScheduler::GetFence(CommandType type) const
{
    return (type == CommandType::Direct) ? m_directFence.Get() : m_copyFence.Get();
}

uint64_t CommandScheduler::IncrementFenceValue(CommandType type)
{
    return (type == CommandType::Direct) ? m_directFenceValue++ : m_copyFenceValue++;
}

void CommandScheduler::WaitQueueIdle(CommandType type)
{
    auto [fence, event, value] = GetFenceEventValue(type);
    WaitFence(fence, event, value);
}

void CommandScheduler::ReleaseCompletedResources()
{
    while (!m_pendingReleases.empty())
    {
        auto& front = m_pendingReleases.front();
        if (front.fence->GetCompletedValue() < front.fenceValue) break;

        m_pendingReleases.pop();
    }
}

bool CommandScheduler::WaitForAllGPU()
{
    WaitFence(m_directFence.Get(), m_fenceDirectEvent, m_directFenceValue - 1);
    WaitFence(m_copyFence.Get(), m_fenceCopyEvent, m_copyFenceValue - 1);
    
    ReleaseCompletedResources();

    return true;
}

ID3D12CommandQueue* CommandScheduler::GetCommandQueue(CommandType type)
{
    return (type == CommandType::Direct)
        ? m_directQueue.Get()
        : m_copyQueue.Get();
}

ID3D12GraphicsCommandList* CommandScheduler::GetCurrentCommandList()
{
    return m_currentContext ? m_currentContext->Get() : nullptr;
}

bool CommandScheduler::CreateQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type, ComPtr<ID3D12CommandQueue>& outQueue)
{
    D3D12_COMMAND_QUEUE_DESC desc{};
    desc.Type = type;
    return SUCCEEDED(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&outQueue)));
}

bool CommandScheduler::CreateFence(ID3D12Device* device, ComPtr<ID3D12Fence>& outFence, HANDLE& outEvent)
{
    if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&outFence))))
        return false;

    outEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    return outEvent != nullptr;
}

void CommandScheduler::WaitFence(ID3D12Fence* fence, HANDLE event, uint64_t value)
{
    if (fence->GetCompletedValue() < value)
    {
        fence->SetEventOnCompletion(value, event);
        WaitForSingleObject(event, INFINITE);
    }
}

std::tuple<ID3D12Fence*, HANDLE, uint64_t> CommandScheduler::GetFenceEventValue(CommandType type) const
{
    if (type == CommandType::Direct)
        return { m_directFence.Get(), m_fenceDirectEvent, m_directFenceValue - 1 };
    else
        return { m_copyFence.Get(), m_fenceCopyEvent, m_copyFenceValue - 1 };
}

HANDLE CommandScheduler::CloseHandleSafe(HANDLE h)
{
    if (h) CloseHandle(h);
    return nullptr;
}