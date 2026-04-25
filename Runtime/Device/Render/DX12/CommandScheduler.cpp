#include "pch.h"
#include "CommandScheduler.h"
#include "d3dx12.h"
#include "CommandQueue.h"
#include "DescriptorAllocation.h"

using Microsoft::WRL::ComPtr;

CommandScheduler::~CommandScheduler()
{
    WaitForAllGPU();
}
CommandScheduler::CommandScheduler() = default;

bool CommandScheduler::Initialize(ID3D12Device* device, 
    uint32_t directPoolSize, uint32_t copyPoolSize, uint32_t computePoolSize)
{
    m_directQueue = make_unique<CommandQueue>();
    m_copyQueue = make_unique<CommandQueue>();
    m_computeQueue = make_unique<CommandQueue>();

    ReturnIfFalse(m_directQueue->Initialize(device, CommandType::Direct, directPoolSize));
    ReturnIfFalse(m_copyQueue->Initialize(device, CommandType::Copy, copyPoolSize));
    ReturnIfFalse(m_computeQueue->Initialize(device, CommandType::Compute, computePoolSize));
    
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

uint64_t CommandScheduler::End(vector<ComPtr<ID3D12Resource>>&& resources)
{
    Assert(m_currentQueue);

    auto fenceValue = m_currentQueue->End(move(resources));
    m_currentQueue = nullptr;
    return fenceValue;
}

uint64_t CommandScheduler::SignalQueue(CommandType type)
{
    return GetQueue(type)->Signal();
}

void CommandScheduler::WaitQueueIdle(CommandType type)
{
    GetQueue(type)->WaitIdle();
}

void CommandScheduler::ReleaseCompletedResources()
{
    m_directQueue->ReleaseCompletedResources();
    m_copyQueue->ReleaseCompletedResources();
    m_computeQueue->ReleaseCompletedResources();
}

void CommandScheduler::WaitForAllGPU()
{
    m_directQueue->WaitForGPU();
    m_copyQueue->WaitForGPU();
    m_computeQueue->WaitForGPU();
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