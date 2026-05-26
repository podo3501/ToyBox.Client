#pragma once
#include "Command/CommandType.h"
#include "RGTypes.h"
#include "GameClient/Service/Render/Desc/TextureBinding.h"
#include "TaskHandle.h"

struct ResourceStateTracker
{
    D3D12_RESOURCE_STATES state{ D3D12_RESOURCE_STATE_COMMON };
};

struct PassNode;
struct RenderPass;
struct TaskContext;
struct CompiledTask;
class CommandList;

class RenderGraph
{
public:
    ~RenderGraph();
    RenderPass& AddPass(const std::string& name, CommandType type);
    std::vector<CompiledTask> Compile();
    RGHandle CreateRGHandle();
    void ImportResource(RGHandle h, RGAccess access);
    void Excute(CommandList& cmd, const vector<CompiledTask>& compiledTasks, TaskContext& ctx);

private:
    int FindPassIndex(const std::string& name);
    std::vector<PassNode> BuildDependencyGraph();
    std::vector<int> TopologicalSort(const std::vector<PassNode>& graph);
    uint32_t CreateLocalTaskID();

    std::vector<RenderPass> m_passes;
    std::unordered_map<uint32_t, ResourceStateTracker> m_statesTracker;
    uint32_t m_nextId{ 1 };
    uint32_t m_nextTaskId{ 1 };
};