#pragma once
#include "TaskNode.h"

class TextureResource;
class MipGenerator;

enum class TaskState
{
    Pending,
    Prepared,
    Dispatched,
    Finalized,
    Completed
};

class MipTask : public TaskNode
{
public:
    ~MipTask();
    MipTask() = delete;
    MipTask(TextureResource* tex, MipGenerator* gen);

    void ExecutePrepare(CommandList& cmd) override;
    void ExecuteDispatch(CommandList& cmd) override
    {
        //if (m_state != TaskState::Prepared)
        //    return;

        //cmd.DependOn(CommandType::Direct, m_prepareFence);

        //m_generator->GenerateMips(cmd, m_texture->GetResource());

        //m_dispatchFence = cmd.GetFence();
        //m_state = TaskState::Dispatched;
    }

    void ExecuteFinalize(CommandList& cmd) override
    {
        //if (m_state != TaskState::Dispatched)
        //    return;

        //cmd.DependOn(CommandType::Compute, m_dispatchFence);

        //CommandUtils::Transition(cmd,
        //    m_texture->GetResource(),
        //    D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        //    D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        //m_state = TaskState::Completed;
    }

private:
    TextureResource* m_texture{ nullptr };
    MipGenerator* m_generator{ nullptr };

    TaskState m_state{ TaskState::Pending };
    uint64_t m_prepareFence{ 0 };
    uint64_t m_dispatchFence{ 0 };
};