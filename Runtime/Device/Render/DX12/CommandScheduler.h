#pragma once
#include <wrl/client.h>
#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

class CommandQueue;

enum class CommandType
{
    Direct, //랜더링
    Copy //리소스 전송
};

class CommandScheduler
{
public:
    ~CommandScheduler();
    CommandScheduler();
    bool Initialize(ID3D12Device* device, int poolSize);
    ID3D12GraphicsCommandList* Begin(CommandType type);
    uint64_t End(vector<ComPtr<ID3D12Resource>>&& resources = {}); // End -> Close + Signal, PendingRelease 등록
    uint64_t SignalQueue(CommandType type);
    void WaitQueueIdle(CommandType type);
    void ReleaseCompletedResources(); // Pending release 체크 후 리소스 해제
    void WaitForAllGPU();
    ID3D12CommandQueue* GetCommandQueue(CommandType type);

private:
    CommandQueue* GetQueue(CommandType type);

    unique_ptr<CommandQueue> m_directQueue;
    unique_ptr<CommandQueue> m_copyQueue;
    CommandQueue* m_currentQueue{ nullptr };
};
