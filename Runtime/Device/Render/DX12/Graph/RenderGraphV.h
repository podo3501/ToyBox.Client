#pragma once
#include "RenderPassV.h"
#include "Task.h"

struct PassNodeV;

class RenderGraphV
{
public:
    ~RenderGraphV();
    RGHandle CreateRGHandle();

    RenderPassV& AddGraphicsPass(std::string name);
    RenderPassV& AddCopyPass(std::string name);
    RenderPassV& AddComputePass(std::string name);
    RenderPassV& AddCpuPass(std::string name);

    std::vector<CompiledTask> Compile();

private:
    RenderPassV& AddPass(std::string name, CommandType type);
    std::vector<PassNodeV> BuildDependencyGraph();
    std::vector<int> TopologicalSort(const std::vector<PassNodeV>& graph);
    uint32_t CreateLocalTaskID();

    std::vector<RenderPassV> m_passes;
    std::unordered_map<uint32_t, ResourceStateTrackerV> m_statesTracker;
    uint32_t m_nextId{ 1 };
    uint32_t m_nextTaskId{ 1 };
};