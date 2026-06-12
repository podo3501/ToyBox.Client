#include "pch.h"
#include "GPUProfiler.h"
#include "Command/CommandScheduler.h"
#include "Command/CommandList.h"

bool GPUProfiler::Initialize(ID3D12Device* device, CommandScheduler* scheduler, uint32_t frameCount)
{
    auto queue = scheduler->GetCommandQueue(CommandType::Direct);
    queue->GetTimestampFrequency(&m_timestampFreq);

    D3D12_QUERY_HEAP_DESC desc = {};
    desc.Type = D3D12_QUERY_HEAP_TYPE_TIMESTAMP;
    desc.Count = frameCount * 2;

    device->CreateQueryHeap(&desc, IID_PPV_ARGS(&m_queryHeap));

    UINT size = sizeof(uint64_t) * frameCount * 2;

    D3D12_HEAP_PROPERTIES hp =
        CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);

    D3D12_RESOURCE_DESC bd =
        CD3DX12_RESOURCE_DESC::Buffer(size);

    device->CreateCommittedResource(
        &hp,
        D3D12_HEAP_FLAG_NONE,
        &bd,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(m_readbackBuffer.GetAddressOf())
    );

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