#pragma once
#include "CommandType.h"
#include "CommandQueue.h"

struct CommandPoolConfig;
class Device;
class CommandList;

class CommandScheduler
{
public:
    ~CommandScheduler();
    CommandScheduler();
    bool Initialize(Device& device, const CommandPoolConfig& config);
    CommandList* Begin(CommandType type);
    FenceID End();
    
    // End -> Close + Signal, PendingRelease 등록
    FenceID SignalQueue(CommandType type);
    void WaitIdle(CommandType type);
    void WaitIdle();
    bool IsFenceComplete(CommandType type, FenceID fenceID);
    CommandQueue* GetQueue(CommandType type) noexcept;

private:
    CommandQueue m_directQueue;
    CommandQueue m_copyQueue;
    CommandQueue m_computeQueue;

    CommandQueue* m_currentQueue{ nullptr };
};
