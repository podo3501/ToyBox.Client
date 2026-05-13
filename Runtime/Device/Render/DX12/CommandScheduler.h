#pragma once
#include <wrl/client.h>
#include "d3dx12.h"
#include "CommandType.h"
#include "FenceTypes.h"
#include "DescriptorAllocation.h"

using Microsoft::WRL::ComPtr;

class CommandList;
class CommandQueue;

class CommandScheduler
{
public:
    ~CommandScheduler();
    CommandScheduler();
    bool Initialize(ID3D12Device* device, 
        uint32_t directCmdPoolSize, 
        uint32_t copyCmdPoolSize, 
        uint32_t computeCmdPoolSize);

    CommandList* Begin(CommandType type);
    uint64_t End();
    
    // End -> Close + Signal, PendingRelease 등록
    uint64_t SignalQueue(CommandType type);
    void WaitIdle(CommandType type);
    void WaitIdle();
    bool IsFenceComplete(CommandType type, uint64_t fenceValue);

    ID3D12CommandQueue* GetCommandQueue(CommandType type);
    QueueFences GetLastSubmittedFences() const noexcept;
    QueueFences GetCompletedFences() const noexcept;

private:
    CommandQueue* GetQueue(CommandType type);

    unique_ptr<CommandQueue> m_directQueue;
    unique_ptr<CommandQueue> m_copyQueue;
    unique_ptr<CommandQueue> m_computeQueue;

    CommandQueue* m_currentQueue{ nullptr };
    vector<DescriptorAllocation> m_pendingDescriptors; //fence때까지 기다렸다 지워질 descriptors 
};
