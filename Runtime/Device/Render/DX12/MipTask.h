#pragma once
#include "TaskNode.h"

class TextureResource;
class MipGenerator;
enum class TaskState;

class MipTask : public TaskNode
{
public:
    ~MipTask();
    MipTask() = delete;
    MipTask(TextureResource* tex, MipGenerator* gen);

    void ExecutePrepare(CommandList& cmd) override;
    void ExecuteDispatch(CommandList& cmd) override;
    void ExecuteFinalize(CommandList& cmd) override;

private:
    TextureResource* m_texture{ nullptr };
    MipGenerator* m_generator{ nullptr };

    TaskState m_state;
    uint64_t m_prepareFence{ 0 };
    uint64_t m_dispatchFence{ 0 };
};