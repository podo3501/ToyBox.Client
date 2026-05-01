#pragma once
#include "CommandType.h"
#include "RGTypes.h"
#include "GameClient/Service/Render/Repository/TextureDesc.h"
#include "TaskHandle.h"

struct ResourceStateTracker
{
    D3D12_RESOURCE_STATES state{ D3D12_RESOURCE_STATE_COMMON };
    TaskHandle lastWriter;
    std::vector<TaskHandle> lastReaders;
};

struct PassNode;
struct BarrierPlan;
struct RenderPass;
struct TaskContext;
struct CompiledTask;
class TaskScheduler;
class CommandList;

class RenderGraph
{
public:
    ~RenderGraph();
    RenderPass& AddPass(const std::string& name, CommandType type);
    std::vector<CompiledTask> Compile(TaskScheduler& scheduler);
    RGResource CreateResource();

private:
    std::vector<PassNode> BuildDependencyGraph();
    std::vector<int> TopologicalSort(const std::vector<PassNode>& graph);

    uint32_t m_nextId{ 1 };
    std::vector<RenderPass> m_passes;
};