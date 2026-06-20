#pragma once
#include "d3dx12.h"
#include <wrl/client.h>
#include "Resource/Resource.h"

class Device;
class CommandList;
class CommandScheduler;
class ResourceFactory;

using Microsoft::WRL::ComPtr;

class GPUProfiler
{
public:
    bool Initialize(Device& device, CommandScheduler& cmdScheduler, 
        ResourceFactory& resFactory, uint32_t frameCount = 2);

    void BeginFrame(CommandList& cmd);
    void EndFrame(CommandList& cmd);
    void Update();
    float GetGpuFrameTimeMs() const { return m_gpuFrameTimeMs; }

private:
    ComPtr<ID3D12QueryHeap> m_queryHeap;
    Resource m_readbackResource;

    uint64_t m_timestampFreq = 0;

    uint32_t m_frameIndex = 0;
    float m_gpuFrameTimeMs = 0.0f;
};