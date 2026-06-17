#include "pch.h"
#include "GPUProfiler.h"
#include "Core/Device.h"
#include "Command/CommandScheduler.h"
#include "Command/CommandList.h"
#include "Factory/ResourceFactory.h"

bool GPUProfiler::Initialize(Device& device, CommandScheduler& cmdScheduler,
    ResourceFactory& resFactory, uint32_t frameCount)
{
    auto queue = cmdScheduler.GetCommandQueue(CommandType::Direct);
    queue->GetTimestampFrequency(&m_timestampFreq);

    constexpr UINT kQueriesPerFrame = 2;

    m_queryHeap = device.CreateQueryHeap(
        D3D12_QUERY_HEAP_TYPE_TIMESTAMP,
        frameCount * kQueriesPerFrame);

    UINT size = sizeof(uint64_t) * frameCount * kQueriesPerFrame;
    m_readbackBuffer = resFactory.CreateReadbackBuffer(size);

    return true;
}

void GPUProfiler::BeginFrame(CommandList& cmd)
{
    uint32_t idx = m_frameIndex % 2;
    cmd->EndQuery(m_queryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, idx * 2);
}

void GPUProfiler::EndFrame(CommandList& cmd)
{
    uint32_t idx = m_frameIndex % 2;

    cmd->EndQuery(m_queryHeap.Get(), D3D12_QUERY_TYPE_TIMESTAMP, idx * 2 + 1);
    
    cmd->ResolveQueryData(
        m_queryHeap.Get(),
        D3D12_QUERY_TYPE_TIMESTAMP,
        idx * 2,
        2,
        m_readbackBuffer.Get(),
        idx * sizeof(uint64_t) * 2
    );

    m_frameIndex++;
}

void GPUProfiler::Update()
{
    uint32_t idx = (m_frameIndex + 1) % 2;

    uint64_t* data = nullptr;

    D3D12_RANGE range = {
        idx * sizeof(uint64_t) * 2,
        (idx + 1) * sizeof(uint64_t) * 2
    };

    if (SUCCEEDED(m_readbackBuffer->Map(0, &range, (void**)&data)))
    {
        uint64_t start = data[idx * 2];
        uint64_t end = data[idx * 2 + 1];

        m_gpuFrameTimeMs =
            float(end - start) * 1000.0f / float(m_timestampFreq);

        m_readbackBuffer->Unmap(0, nullptr);
    }
}