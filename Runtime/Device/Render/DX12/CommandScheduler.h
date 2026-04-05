#pragma once
#include <wrl/client.h>
#include "CommandContext.h"

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
    uint64_t End(std::vector<ComPtr<ID3D12Resource>>&& resources = {}); // End -> Close + Signal, PendingRelease 등록
    uint64_t SignalQueue(CommandType type);
    void WaitQueueIdle(CommandType type);
    void ReleaseCompletedResources(); // Pending release 체크 후 리소스 해제
    bool WaitForAllGPU();

    ID3D12CommandQueue* GetCommandQueue(CommandType type);
    ID3D12GraphicsCommandList* GetCurrentCommandList();

private:
    struct PendingRelease
    {
        ID3D12Fence* fence{ nullptr };
        uint64_t fenceValue{ 0 };
        vector<ComPtr<ID3D12Resource>> resources;
    };

    bool CreateQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type, ComPtr<ID3D12CommandQueue>& outQueue);
    bool CreateFence(ID3D12Device* device, ComPtr<ID3D12Fence>& outFence, HANDLE& outEvent);
    uint64_t SignalFence(ID3D12CommandQueue* queue, CommandType type);
    ID3D12Fence* GetFence(CommandType type) const;
    CommandContext* GetAvailableContext(CommandType type);
    uint64_t IncrementFenceValue(CommandType type);
    void WaitFence(ID3D12Fence* fence, HANDLE event, uint64_t value);
    tuple<ID3D12Fence*, HANDLE, uint64_t> GetFenceEventValue(CommandType type) const;
    HANDLE CloseHandleSafe(HANDLE h);

    ComPtr<ID3D12CommandQueue> m_directQueue;
    ComPtr<ID3D12CommandQueue> m_copyQueue;

    ComPtr<ID3D12Fence> m_directFence;
    ComPtr<ID3D12Fence> m_copyFence;

    uint64_t m_directFenceValue = 1;
    uint64_t m_copyFenceValue = 1;

    HANDLE m_fenceDirectEvent = nullptr;
    HANDLE m_fenceCopyEvent = nullptr;

    vector<unique_ptr<CommandContext>> m_directPool;
    vector<unique_ptr<CommandContext>> m_copyPool;

    size_t m_nextDirect{ 0 };
    size_t m_nextCopy{ 0 };
    CommandContext* m_currentContext{ nullptr };
    CommandType m_currentType{ CommandType::Direct };
    queue<PendingRelease> m_pendingReleases;
};
