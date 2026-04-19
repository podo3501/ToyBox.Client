#include "pch.h"
#include "MipTask.h"
#include "CommandList.h"
#include "TextureResource.h"
#include "MipGenerator.h"
#include "CommandUtils.h"

MipTask::~MipTask() = default;
MipTask::MipTask(TextureResource* tex, MipGenerator* gen) : 
	m_texture(tex), m_generator(gen) 
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

//void MipTask::ExecuteDispatch(CommandList& cmd)
//{
//    if (m_state != TaskState::Prepared)
//        return;
//
//    cmd.DependOn(CommandType::Direct, m_prepareFence);
//    m_generator->GenerateMips(cmd, m_texture->Get());
//
//    m_dispatchFence = cmd.GetFence();
//    m_state = TaskState::Dispatched;
//}