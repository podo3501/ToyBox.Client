#include "pch.h"
#include "FrameProfiler.h"
#include "Core/Device.h"
#include "Command/CommandScheduler.h"
#include "Command/CommandList.h"
#include "Factory/ResourceFactory.h"

bool FrameProfiler::Initialize(Device& device, CommandScheduler& cmdScheduler,
    ResourceFactory& resFactory, uint32_t frameCount)
{
    m_frameCount = frameCount;

    auto queue = cmdScheduler.GetCommandQueue(CommandType::Direct);
    queue->GetTimestampFrequency(&m_timestampFreq);

    m_queryHeap = device.CreateQueryHeap(
        D3D12_QUERY_HEAP_TYPE_TIMESTAMP,
        frameCount * QueriesPerFrame);

    const UINT64 size = sizeof(uint64_t) * frameCount * QueriesPerFrame;
    m_readbackResource = resFactory.CreateResource(size, ResInitType::Readback);

    return true;
}

void FrameProfiler::BeginFrame(CommandList& cmd, uint64_t frameIndex)
{
    m_currentSlot = static_cast<UINT>(frameIndex % m_frameCount);

    cmd->EndQuery(
        m_queryHeap.Get(), 
        D3D12_QUERY_TYPE_TIMESTAMP, 
        m_currentSlot * QueriesPerFrame);

    m_cpuStart = std::chrono::high_resolution_clock::now();
}

void FrameProfiler::EndFrame(CommandList& cmd)
{
    cmd->EndQuery(
        m_queryHeap.Get(), 
        D3D12_QUERY_TYPE_TIMESTAMP, 
        m_currentSlot * QueriesPerFrame + 1);
    
    cmd->ResolveQueryData(
        m_queryHeap.Get(),
        D3D12_QUERY_TYPE_TIMESTAMP,
        m_currentSlot * QueriesPerFrame,
        QueriesPerFrame,
        m_readbackResource.Get(),
        m_currentSlot * sizeof(uint64_t) * QueriesPerFrame
    );

    auto now = std::chrono::high_resolution_clock::now();
    m_cpuFrameTimeMs = std::chrono::duration<float, std::milli>(now - m_cpuStart).count();
}

void FrameProfiler::Update(uint64_t frameIndex)
{
    const auto slot = static_cast<UINT>((frameIndex + m_frameCount - 1) % m_frameCount); // 이전 프레임의 GPU 측정 결과를 읽는다.
    uint64_t* data = nullptr;

    D3D12_RANGE range = 
    { 
        slot * sizeof(uint64_t) * QueriesPerFrame,
        (slot + 1) * sizeof(uint64_t) * QueriesPerFrame
    };

    if (SUCCEEDED(m_readbackResource->Map(
        0, 
        &range, 
        reinterpret_cast<void**>(&data))))
    {
        const UINT queryIndex = slot * QueriesPerFrame;

        uint64_t start = data[queryIndex];
        uint64_t end = data[queryIndex + 1];
        m_gpuFrameTimeMs = float(end - start) * 1000.0f / float(m_timestampFreq);

        m_readbackResource->Unmap(0, nullptr);
    }
}