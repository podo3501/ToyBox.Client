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

bool CommandScheduler::Initialize(ID3D12Device* device, uint32_t directPoolSize, uint32_t copyPoolSize)
{
    m_directQueue = make_unique<CommandQueue>();
    m_copyQueue = make_unique<CommandQueue>();

    ReturnIfFalse(m_directQueue->Initialize(device, CommandType::Direct, directPoolSize));
    ReturnIfFalse(m_copyQueue->Initialize(device, CommandType::Copy, copyPoolSize));
    
    return true;
}

CommandList* CommandScheduler::Begin(CommandType type)
{
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
}

void CommandScheduler::WaitForAllGPU()
{
    m_directQueue->WaitForGPU();
    m_copyQueue->WaitForGPU();
}

ID3D12CommandQueue* CommandScheduler::GetCommandQueue(CommandType type)
{
    return GetQueue(type)->GetQueue();
}

CommandQueue* CommandScheduler::GetQueue(CommandType type)
{
    return (type == CommandType::Direct)
        ? m_directQueue.get()
        : m_copyQueue.get();
}

QueueFences CommandScheduler::GetLastSubmittedFences() const noexcept
{
    return QueueFences{
        m_directQueue->GetLastSubmittedFence(),
        m_copyQueue->GetLastSubmittedFence() };
}

QueueFences CommandScheduler::GetCompletedFences() const noexcept
{
    return QueueFences{
        m_directQueue->GetCompletedFence(),
        m_copyQueue->GetCompletedFence() };
}