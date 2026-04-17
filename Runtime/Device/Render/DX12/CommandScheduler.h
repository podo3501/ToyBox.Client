#pragma once
#include <wrl/client.h>
#include "d3dx12.h"
#include "CommandType.h"
#include "FenceTypes.h"
#include "DescriptorAllocation.h"

using Microsoft::WRL::ComPtr;

class CommandQueue;

class CommandScheduler
{
public:
    ~CommandScheduler();
    CommandScheduler();
    bool Initialize(ID3D12Device* device, uint32_t directPoolSize, uint32_t copyPoolSize);
    ID3D12GraphicsCommandList* Begin(CommandType type);
    uint64_t End(vector<ComPtr<ID3D12Resource>>&& resources = {});
    void EnqueueDeferredDescriptors(DescriptorAllocation&& descriptor);
    
    // End -> Close + Signal, PendingRelease 등록
    uint64_t SignalQueue(CommandType type);
    void WaitQueueIdle(CommandType type);
    void ReleaseCompletedResources(); // Pending release 체크 후 리소스 해제
    void WaitForAllGPU();
    ID3D12CommandQueue* GetCommandQueue(CommandType type);
    SubmittedFences GetLastSubmittedFences() const noexcept;
    CompletedFences GetCompletedFences() const noexcept;

private:
    void DeferredFreeDescriptors();
    CommandQueue* GetQueue(CommandType type);

    unique_ptr<CommandQueue> m_directQueue;
    unique_ptr<CommandQueue> m_copyQueue;
    CommandQueue* m_currentQueue{ nullptr };
    vector<DescriptorAllocation> m_pendingDescriptors; //fence때까지 기다렸다 지워질 descriptors 
};
