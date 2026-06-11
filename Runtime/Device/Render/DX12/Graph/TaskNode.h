#pragma once 
#include <cstdint>

class CommandList;

class TaskNode
{
public:
    virtual ~TaskNode() = default;

    // phase execution
    virtual void ExecutePrepare(CommandList& cmd) {}
    virtual void ExecuteDispatch(CommandList& cmd) {}
    virtual void ExecuteFinalize(CommandList& cmd) {}

    // dependency
    void SetPrepareFence(uint64_t fence) { m_prepareFence = fence; }
    void SetDispatchFence(uint64_t fence) { m_dispatchFence = fence; }

    uint64_t GetPrepareFence() const { return m_prepareFence; }
    uint64_t GetDispatchFence() const { return m_dispatchFence; }

    bool IsCompleted() const { return m_completed; }
    void SetCompleted(bool v) { m_completed = v; }

protected:
    uint64_t m_prepareFence{ 0 };
    uint64_t m_dispatchFence{ 0 };

    bool m_completed{ false };
};