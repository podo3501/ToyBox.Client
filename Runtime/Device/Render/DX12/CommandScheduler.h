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
    void ReleaseCompletedResources(); // Pending release 체크 후 리소스 해제
    bool WaitForAllGPU();

    ID3D12CommandQueue* GetCommandQueue(CommandType type);
    ///ID3D12GraphicsCommandList* GetCommandList(CommandType type);
    ID3D12GraphicsCommandList* GetCurrentCommandList();

private:
    struct PendingRelease
    {
        CommandContext* context;
        uint64_t fenceValue;
        vector<ComPtr<ID3D12Resource>> resources;
    };

    //CommandContext* GetCommandContext(CommandType type);

    //CommandContext m_direct;
    //CommandContext m_copy;

    CommandContext* GetAvailableContext(CommandType type);

    ComPtr<ID3D12CommandQueue> m_directQueue;
    ComPtr<ID3D12CommandQueue> m_copyQueue;

    vector<unique_ptr<CommandContext>> m_directPool;
    vector<unique_ptr<CommandContext>> m_copyPool;

    size_t m_nextDirect{ 0 };
    size_t m_nextCopy{ 0 };
    CommandContext* m_currentContext{ nullptr };
    CommandType m_currentType{ CommandType::Direct };
    queue<PendingRelease> m_pendingReleases;
};
