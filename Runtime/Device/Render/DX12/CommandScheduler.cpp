#include "pch.h"
#include "CommandScheduler.h"
#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

CommandScheduler::~CommandScheduler() = default;
CommandScheduler::CommandScheduler() = default;

bool CommandScheduler::Initialize(ID3D12Device* device, int poolSize)
{
    /*ReturnIfFalse(m_direct.Initialize(device, D3D12_COMMAND_LIST_TYPE_DIRECT));
    ReturnIfFalse(m_copy.Initialize(device, D3D12_COMMAND_LIST_TYPE_COPY));

    return true;*/

    D3D12_COMMAND_QUEUE_DESC desc{};
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    ReturnIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_directQueue)));

    desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
    ReturnIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_copyQueue)));

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

        if (ctx->GetCompletedValue() >= ctx->GetLastSignaledFence())
            return ctx;

    }
    return nullptr; // 사용 가능한 context 없음
}

ID3D12GraphicsCommandList* CommandScheduler::Begin(CommandType type)
{
    if (m_currentContext) return nullptr;

    CommandContext* cmd = GetAvailableContext(type);
    if (!cmd) return nullptr;

    if (!cmd->Reset()) return nullptr;
    m_currentContext = cmd;
    m_currentType = type;

    return cmd->Get();
}

uint64_t CommandScheduler::End(std::vector<ComPtr<ID3D12Resource>>&& resources)
{
    if (!m_currentContext) return 0;

    CommandContext* context = m_currentContext;
    ReturnIfFalse(context->Close());

    //uint64_t fenceValue = context->Signal(); // GPU에 fence signal
    ID3D12CommandQueue* queue = GetCommandQueue(m_currentType);
    ID3D12CommandList* lists[] = { context->Get() };
    queue->ExecuteCommandLists(1, lists);

    uint64_t fenceValue = context->NextFenceValue();
    ReturnIfFailed(queue->Signal(context->GetFence(), fenceValue));

    if (!resources.empty())
        m_pendingReleases.push({ context, fenceValue, move(resources) });

    m_currentContext = nullptr;
    return fenceValue;
}

void CommandScheduler::ReleaseCompletedResources()
{
    while (!m_pendingReleases.empty())
    {
        auto& front = m_pendingReleases.front();
        uint64_t completed = front.context->GetCompletedValue();
        if (completed < front.fenceValue) break;

        m_pendingReleases.pop(); // ComPtr 벡터 파괴. 자동 Release
    }
}

bool CommandScheduler::WaitForAllGPU()
{
    /*ReturnIfFalse(m_direct.WaitForFence(m_direct.GetLastSignaledFence()));
    ReturnIfFalse(m_copy.WaitForFence(m_copy.GetLastSignaledFence()));*/

    for (auto& ctx : m_directPool) 
        ReturnIfFalse(ctx->WaitForFence(ctx->GetLastSignaledFence()));
    for (auto& ctx : m_copyPool) 
        ReturnIfFalse(ctx->WaitForFence(ctx->GetLastSignaledFence()));

    ReleaseCompletedResources(); // ReleasePending 모두 처리

    return true;
}

//CommandContext* CommandScheduler::GetCommandContext(CommandType type)
//{
//    switch (type)
//    {
//    case CommandType::Direct: return &m_direct;
//    case CommandType::Copy:   return &m_copy;
//    }
//    return nullptr;
//}

ID3D12CommandQueue* CommandScheduler::GetCommandQueue(CommandType type)
{
    //switch (type)
    //{
    //case CommandType::Direct: return m_direct.GetQueue();
    //case CommandType::Copy:   return m_copy.GetQueue();
    //}
    //return nullptr;

    switch (type)
    {
    case CommandType::Direct: return m_directQueue.Get();
    case CommandType::Copy:   return m_copyQueue.Get();
    }
    return nullptr;
}

ID3D12GraphicsCommandList* CommandScheduler::GetCurrentCommandList()
{
    if (!m_currentContext) return nullptr;
    return m_currentContext->Get();
}