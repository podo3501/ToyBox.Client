#pragma once
#include <wrl/client.h>
#include "d3dx12.h"

using Microsoft::WRL::ComPtr;

class CommandListEntry;

class CommandQueue
{
public:
    ~CommandQueue();
    CommandQueue();

    bool Initialize(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type, int poolSize);
    ID3D12GraphicsCommandList* Begin();
    uint64_t End(vector<ComPtr<ID3D12Resource>>&& resources);

    uint64_t Signal();
    void WaitIdle();
    void ReleaseCompletedResources();
    void WaitForGPU();

    ID3D12CommandQueue* GetQueue() const { return m_queue.Get(); }

private:
    struct PendingRelease
    {
        uint64_t fenceValue{ 0 };
        std::vector<ComPtr<ID3D12Resource>> resources;
    };

private:
    bool CreateQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
    bool CreateFence(ID3D12Device* device);
    CommandListEntry* GetAvailableCommandListEntry();
    void WaitFence(uint64_t value);

    ComPtr<ID3D12CommandQueue> m_queue;
    ComPtr<ID3D12Fence> m_fence;

    uint64_t m_fenceValue{ 0 };
    HANDLE m_event{ nullptr };

    vector<unique_ptr<CommandListEntry>> m_pool;
    size_t m_next{ 0 }; //command pool에서 다음에 어떤 command를 사용할지.

    CommandListEntry* m_currentCmdEntry{ nullptr };

    queue<PendingRelease> m_pendingReleases;
};

