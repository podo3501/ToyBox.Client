#include "pch.h"
#include "CommandScheduler.h"
#include "CommandQueue.h"

using Microsoft::WRL::ComPtr;

CommandScheduler::~CommandScheduler()
{
    WaitIdle();
}
CommandScheduler::CommandScheduler() = default;

bool CommandScheduler::Initialize(ID3D12Device* device, 
    uint32_t directCmdPoolSize, uint32_t copyCmdPoolSize, uint32_t computeCmdPoolSize)
{
    m_directQueue = make_unique<CommandQueue>();
    m_copyQueue = make_unique<CommandQueue>();
    m_computeQueue = make_unique<CommandQueue>();

    ReturnIfFalse(m_directQueue->Initialize(device, CommandType::Direct, directCmdPoolSize));
    ReturnIfFalse(m_copyQueue->Initialize(device, CommandType::Copy, copyCmdPoolSize));
    ReturnIfFalse(m_computeQueue->Initialize(device, CommandType::Compute, computeCmdPoolSize));
    
    return true;
}

CommandList* CommandScheduler::Begin(CommandType type)
{
    if (type == CommandType::None) return nullptr;

    Assert(!m_currentQueue);

    m_currentQueue = GetQueue(type);
    auto cmd = m_currentQueue->Begin();
    if (!cmd) m_currentQueue = nullptr;

    return cmd;
}

uint64_t CommandScheduler::End()
{
    Assert(m_currentQueue);

    auto fenceValue = m_currentQueue->End();
    m_currentQueue = nullptr;
    return fenceValue;
}

uint64_t CommandScheduler::SignalQueue(CommandType type)
{
    return GetQueue(type)->Signal();
}

void CommandScheduler::WaitIdle(CommandType type)
{
    GetQueue(type)->WaitIdle();
}

void CommandScheduler::WaitIdle()
{
    m_directQueue->WaitIdle();
    m_copyQueue->WaitIdle();
    m_computeQueue->WaitIdle();
}

bool CommandScheduler::IsFenceComplete(CommandType type, uint64_t fenceValue)
{
    if (fenceValue == 0)
        return true;

    return GetQueue(type)->GetCompletedFence() >= fenceValue;
}

ID3D12CommandQueue* CommandScheduler::GetCommandQueue(CommandType type)
{
    return GetQueue(type)->GetQueue();
}

CommandQueue* CommandScheduler::GetQueue(CommandType type)
{
    switch (type)
    {
    case CommandType::Direct: return m_directQueue.get();
    case CommandType::Copy: return m_copyQueue.get();
    case CommandType::Compute: return m_computeQueue.get();
    default:
        return nullptr;
    }
}

QueueFences CommandScheduler::GetLastSubmittedFences() const noexcept
{
    return QueueFences{
        m_directQueue->GetLastSubmittedFence(),
        m_copyQueue->GetLastSubmittedFence(),
        m_computeQueue->GetLastSubmittedFence()};
}

QueueFences CommandScheduler::GetCompletedFences() const noexcept
{
    return QueueFences{
        m_directQueue->GetCompletedFence(),
        m_copyQueue->GetCompletedFence(),
        m_computeQueue->GetCompletedFence() };
}