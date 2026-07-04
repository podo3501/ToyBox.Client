#include "pch.h"
#include "CommandScheduler.h"
#include "GameClient/Service/Render/RenderConfig.h"

using Microsoft::WRL::ComPtr;

CommandScheduler::~CommandScheduler() { WaitIdle(); }
CommandScheduler::CommandScheduler() = default;

bool CommandScheduler::Initialize(Device& device, const CommandPoolConfig& config)
{
    ReturnIfFalse(m_directQueue.Initialize(device, CommandType::Direct, config.direct));
    ReturnIfFalse(m_copyQueue.Initialize(device, CommandType::Copy, config.copy));
    ReturnIfFalse(m_computeQueue.Initialize(device, CommandType::Compute, config.compute));
    
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
    m_directQueue.WaitIdle();
    m_copyQueue.WaitIdle();
    m_computeQueue.WaitIdle();
}

bool CommandScheduler::IsFenceComplete(CommandType type, uint64_t fenceValue)
{
    if (fenceValue == 0)
        return true;

    return GetQueue(type)->GetCompletedFence() >= fenceValue;
}

CommandQueue* CommandScheduler::GetQueue(CommandType type) noexcept
{
    switch (type)
    {
    case CommandType::Direct: return &m_directQueue;
    case CommandType::Copy: return &m_copyQueue;
    case CommandType::Compute: return &m_computeQueue;
    }

    return nullptr;
}