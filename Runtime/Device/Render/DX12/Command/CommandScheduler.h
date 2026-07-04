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
    uint64_t End();
    
    // End -> Close + Signal, PendingRelease µî·Ï
    uint64_t SignalQueue(CommandType type);
    void WaitIdle(CommandType type);
    void WaitIdle();
    bool IsFenceComplete(CommandType type, uint64_t fenceValue);
    CommandQueue* GetQueue(CommandType type) noexcept;

private:
    CommandQueue m_directQueue;
    CommandQueue m_copyQueue;
    CommandQueue m_computeQueue;

    CommandQueue* m_currentQueue{ nullptr };
};
