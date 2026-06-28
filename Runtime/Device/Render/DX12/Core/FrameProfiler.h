#pragma once
#include "d3dx12.h"
#include <wrl/client.h>
#include "Resource/Resource.h"

class Device;
class CommandList;
class CommandScheduler;
class ResourceFactory;

using Microsoft::WRL::ComPtr;

class FrameProfiler
{
public:
    bool Initialize(Device& device, CommandScheduler& cmdScheduler, 
        ResourceFactory& resFactory, uint32_t frameCount = 2);

    void BeginFrame(CommandList& cmd, uint64_t frameIndex);
    void EndFrame(CommandList& cmd);
    void Update(uint64_t frameIndex);

    float GetCpuFrameTimeMs() const { return m_cpuFrameTimeMs; }
    float GetGpuFrameTimeMs() const { return m_gpuFrameTimeMs; }

private:
    static constexpr UINT QueriesPerFrame = 2;

    ComPtr<ID3D12QueryHeap> m_queryHeap;
    Resource m_readbackResource;

    uint64_t m_timestampFreq{ 0 };

    uint32_t m_frameCount{ 0 };
    UINT  m_currentSlot{ 0 };
    float m_gpuFrameTimeMs{ 0.0f };

    std::chrono::high_resolution_clock::time_point m_cpuStart;
    float m_cpuFrameTimeMs{ 0.0f };
};