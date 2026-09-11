#pragma once
#include <wrl/client.h>
#include "CommandType.h"

using Microsoft::WRL::ComPtr;

struct ID3D12DescriptorHeap;
class Device;
class CommandList;

class CommandQueue
{
public:
    ~CommandQueue();
    CommandQueue();

    bool Initialize(
        Device& device, 
        ID3D12DescriptorHeap* bindlessHeap,
        CommandType type, 
        uint32_t cmdPoolSize);

    CommandList* Begin();
    FenceID End();

    // Render 전용: Begin()~End() 사이에서만 유효
    std::vector<CommandList*> BeginParallel(size_t count);
    CommandList* EndParallel(std::span<CommandList*> cmdLists);

    void AbortFrame();
    FenceID Signal();
    void WaitIdle();

    ID3D12CommandQueue* GetQueue() const { return m_queue.Get(); }
    FenceID GetCurrentFence() const noexcept { return m_fenceID; }
    FenceID GetCompletedFence() const noexcept { return m_fence->GetCompletedValue(); }

private:
    bool CreateQueue(Device& device, CommandType type);
    bool CreateFence(Device& device);
    void PrepareCommandList(CommandList& cmd);
    CommandList* GetAvailableCommandList();
    void WaitFence(FenceID fenceID);

    ID3D12DescriptorHeap* m_bindlessHeap{ nullptr };
    ComPtr<ID3D12CommandQueue> m_queue;
    ComPtr<ID3D12Fence> m_fence;

    FenceID m_fenceID{ InvalidFenceID };
    HANDLE m_event{ nullptr };

    vector<CommandList> m_pool;
    size_t m_next{ 0 }; //command pool에서 다음에 어떤 command를 사용할지.
    FenceID m_lastSubmittedFence{ InvalidFenceID }; //여기까지 명령어가 들어가 있는 펜스값. GetCompletedValue() 값은 실제로 다 끝난 펜스값.

    CommandList* m_currentCmdEntry{ nullptr };
    std::vector<CommandList*> m_pendingSubmission;
};

