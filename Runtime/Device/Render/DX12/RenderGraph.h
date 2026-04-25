#pragma once
#include "CommandType.h"
#include "RGTypes.h"
#include "GameClient/Service/Render/TextureDesc.h"
#include "TaskHandle.h"

struct ResourceState
{
    D3D12_RESOURCE_STATES state = D3D12_RESOURCE_STATE_COMMON;
    TaskHandle lastWriter;
    std::vector<TaskHandle> lastReaders;
};

struct BarrierPlan;
struct RenderPass;
struct TaskContext;
class TaskScheduler;
class CommandList;

class RenderGraph
{
public:
    ~RenderGraph();
    //RGTexture Import(ID3D12Resource* resource);
    RenderPass& AddPass(const std::string& name, CommandType type);
    void Compile(TaskScheduler& scheduler);
    RGTexture CreateTexture(const TextureDesc& desc);

private:
    std::vector<BarrierPlan> BuildBarriers(CommandType cmdType, const RenderPass& pass);

    uint32_t m_nextId{ 1 };

    std::vector<RenderPass> m_passes;
    std::unordered_map<uint32_t, ID3D12Resource*> m_imported;
    std::unordered_map<uint32_t, TextureDesc> m_textureDescs;
    std::unordered_map<uint32_t, ResourceState> m_states;
};