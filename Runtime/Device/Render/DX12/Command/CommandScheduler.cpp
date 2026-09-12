#include "pch.h"
#include "CommandScheduler.h"
#include "GameClient/Service/Render/RenderConfig.h"

using Microsoft::WRL::ComPtr;

CommandScheduler::~CommandScheduler() { WaitIdle(); }
CommandScheduler::CommandScheduler() = default;

bool CommandScheduler::Initialize(
    Device& device, 
    ID3D12DescriptorHeap* bindlessHeap, 
    const CommandPoolConfig& config)
{
    ReturnIfFalse(m_directQueue.Initialize(device, bindlessHeap, CommandType::Direct, config.direct));
    ReturnIfFalse(m_copyQueue.Initialize(device, nullptr, CommandType::Copy, config.copy));
    ReturnIfFalse(m_computeQueue.Initialize(device, bindlessHeap, CommandType::Compute, config.compute));
    
    return true;
}

CommandList* CommandScheduler::Begin(uint32_t slot)
{
    Assert(!m_currentQueue);

    m_currentQueue = GetQueue(CommandType::Direct);
    auto cmd = m_currentQueue->Begin(slot);
    if (!cmd) m_currentQueue = nullptr;

    return cmd;
}

FenceID CommandScheduler::End()
{
    Assert(m_currentQueue);

    auto fenceID = m_currentQueue->End();
    m_currentQueue = nullptr;
    return fenceID;
}

std::vector<CommandList*> CommandScheduler::BeginParallel(size_t count)
{
    Assert(m_currentQueue); // Begin()으로 연 프레임 도중이어야 함
    return m_currentQueue->BeginParallel(count);
}

CommandList* CommandScheduler::EndParallel(std::span<CommandList*> cmds)
{
    Assert(m_currentQueue);
    return m_currentQueue->EndParallel(cmds);
}

void CommandScheduler::AbortFrame()
{
    Assert(m_currentQueue); // Begin()으로 연 프레임 도중이어야 함

    m_currentQueue->AbortFrame();
    m_currentQueue = nullptr; // End()와 동일하게 "프레임 종료" 상태로 되돌림
}

FenceID CommandScheduler::SignalQueue(CommandType type)
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

bool CommandScheduler::IsFenceComplete(CommandType type, FenceID fenceID)
{
    if (fenceID == 0)
        return true;

    return GetQueue(type)->GetCompletedFence() >= fenceID;
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