#include "pch.h"
#include "CommandScheduler.h"
#include "d3dx12.h"
#include "CommandQueue.h"
#include "DescriptorAllocation.h"

using Microsoft::WRL::ComPtr;

CommandScheduler::~CommandScheduler() = default;
CommandScheduler::CommandScheduler() = default;

bool CommandScheduler::Initialize(ID3D12Device* device, uint32_t directPoolSize, uint32_t copyPoolSize)
{
    m_directQueue = make_unique<CommandQueue>();
    m_copyQueue = make_unique<CommandQueue>();

    ReturnIfFalse(m_directQueue->Initialize(device, CommandType::Direct, directPoolSize));
    ReturnIfFalse(m_copyQueue->Initialize(device, CommandType::Copy, copyPoolSize));
    
    return true;
}

ID3D12GraphicsCommandList* CommandScheduler::Begin(CommandType type)
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
    DeferredFreeDescriptors();

    m_currentQueue = nullptr;
    return fenceValue;
}

void CommandScheduler::EnqueueDeferredDescriptors(DescriptorAllocation&& descriptor)
{
    m_pendingDescriptors.emplace_back(move(descriptor)); //end때의 fence 값으로 지워야 하기 때문에 일단 모아놓음.
}

void CommandScheduler::DeferredFreeDescriptors()
{
    SubmittedFences fences = GetLastSubmittedFences();
    for (auto& d : m_pendingDescriptors)
        d.SetDeferredContext(fences); // descriptors는 여기서 scope 끝 -> destructor -> DeferredFree 호출됨
    m_pendingDescriptors.clear();
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

SubmittedFences CommandScheduler::GetLastSubmittedFences() const noexcept
{
    return SubmittedFences{
        m_directQueue->GetLastSubmittedFence(),
        m_copyQueue->GetLastSubmittedFence() };
}

CompletedFences CommandScheduler::GetCompletedFences() const noexcept
{
    return CompletedFences{
        m_directQueue->GetCompletedFence(),
        m_copyQueue->GetCompletedFence() };
}