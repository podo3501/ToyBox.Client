#include "pch.h"
#include "MipTask.h"
#include "CommandList.h"
#include "TextureResource.h"
#include "MipGenerator.h"
#include "CommandUtils.h"

enum class TaskState
{
    Pending,
    Prepared,
    Dispatched,
    Finalized,
    Completed
};

MipTask::~MipTask() = default;
MipTask::MipTask(TextureResource* tex, MipGenerator* gen) : 
	m_texture(tex), m_generator(gen), m_state{ TaskState::Pending }
{}

void MipTask::ExecutePrepare(CommandList& cmd)
{
    if (m_state != TaskState::Pending)
        return;

    CommandUtils::Transition(cmd, m_texture->Get(),
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

    m_prepareFence = cmd.GetFence();
    m_state = TaskState::Prepared;
}

void MipTask::ExecuteDispatch(CommandList& cmd)
{
    if (m_state != TaskState::Prepared)
        return;

    cmd.DependOn(CommandType::Direct, m_prepareFence);
    m_generator->GenerateMips(cmd, m_texture->Get());

    m_dispatchFence = cmd.GetFence();
    m_state = TaskState::Dispatched;
}

void MipTask::ExecuteFinalize(CommandList& cmd)
{
    if (m_state != TaskState::Dispatched)
        return;

    cmd.DependOn(CommandType::Compute, m_dispatchFence);
    CommandUtils::Transition(cmd, m_texture->Get(),
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    m_state = TaskState::Completed;
}