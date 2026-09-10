#pragma once
#include "CommandType.h"
#include "CommandQueue.h"

struct ID3D12DescriptorHeap;
struct CommandPoolConfig;
class Device;
class CommandList;

class CommandScheduler
{
public:
    ~CommandScheduler();
    CommandScheduler();
    bool Initialize(
        Device& device, 
        ID3D12DescriptorHeap* bindlessHeap, 
        const CommandPoolConfig& config);
    CommandList* Begin(CommandType type);
    FenceID End();

    // Render 전용: Begin()~End() 사이에서만 유효
    std::vector<CommandList*> BeginParallel(size_t count);
    CommandList* EndParallel(std::span<CommandList*> cmds);
    
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
