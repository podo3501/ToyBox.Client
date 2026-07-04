#pragma once
#include <wrl/client.h>
#include "CommandType.h"

using Microsoft::WRL::ComPtr;

class Device;
class CommandList;

class CommandQueue
{
public:
    ~CommandQueue();
    CommandQueue();

    bool Initialize(Device& device, CommandType type, uint32_t cmdPoolSize);
    CommandList* Begin();
    uint64_t End();

    uint64_t Signal();
    void WaitIdle();

    ID3D12CommandQueue* GetQueue() const { return m_queue.Get(); }
    uint64_t GetCurrentFence() const noexcept { return m_fenceValue; }
    uint64_t GetCompletedFence() const noexcept { return m_fence->GetCompletedValue(); }

private:
    bool CreateQueue(Device& device, CommandType type);
    bool CreateFence(Device& device);
    CommandList* GetAvailableCommandList();
    void WaitFence(uint64_t value);

    ComPtr<ID3D12CommandQueue> m_queue;
    ComPtr<ID3D12Fence> m_fence;

    uint64_t m_fenceValue{ 0 };
    HANDLE m_event{ nullptr };

    vector<CommandList> m_pool;
    size_t m_next{ 0 }; //command pool에서 다음에 어떤 command를 사용할지.
    uint64_t m_lastSubmittedFence{ 0 }; //여기까지 명령어가 들어가 있는 펜스값. GetCompletedValue() 값은 실제로 다 끝난 펜스값.

    CommandList* m_currentCmdEntry{ nullptr };
};

