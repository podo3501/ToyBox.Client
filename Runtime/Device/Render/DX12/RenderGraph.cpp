#include "pch.h"
#include "RenderGraph.h"
#include "RenderPass.h"
#include "TaskScheduler.h"
#include "CommandUtils.h"
#include "CommandList.h"
#include <unordered_set>

static D3D12_RESOURCE_STATES AccessToState(CommandType cmdType, RGAccess access)
{
    if (access == RGAccess::CopyDest)
    {
        if (cmdType == CommandType::Copy) return D3D12_RESOURCE_STATE_COMMON; //copy queue 일때에는 common에서 처리하기 때문이다.
        return D3D12_RESOURCE_STATE_COPY_DEST;
    }

    switch (access)
    {
    case RGAccess::SRV: return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    case RGAccess::UAV: return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    //case RGAccess::CopySrc: return D3D12_RESOURCE_STATE_COPY_SOURCE;
    //case RGAccess::CopyDst: return D3D12_RESOURCE_STATE_COPY_DEST;
    //case RGAccess::RTV: return D3D12_RESOURCE_STATE_RENDER_TARGET;
    default: return D3D12_RESOURCE_STATE_COMMON;
    }
}

RenderGraph::~RenderGraph() = default;
RenderPass& RenderGraph::AddPass(const std::string& name, CommandType type)
{
    m_passes.emplace_back();
    auto& pass = m_passes.back();

    pass.name = name;
    pass.type = type;

    return pass;
}

static CommandType ResolveCommandType(CommandType type,
    D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
    if (type == CommandType::None) 
        return CommandType::Direct;

    auto isDirectOnly = [](D3D12_RESOURCE_STATES s) {
        return
            (s & D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE) ||
            (s & D3D12_RESOURCE_STATE_RENDER_TARGET) ||
            (s & D3D12_RESOURCE_STATE_DEPTH_WRITE) ||
            (s & D3D12_RESOURCE_STATE_DEPTH_READ) ||
            (s & D3D12_RESOURCE_STATE_RESOLVE_DEST) ||
            (s & D3D12_RESOURCE_STATE_RESOLVE_SOURCE) ||
            (s & D3D12_RESOURCE_STATE_PRESENT) ||
            (s & D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER) ||
            (s & D3D12_RESOURCE_STATE_INDEX_BUFFER) ||
            (s & D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT);
        };

    if (isDirectOnly(before) || isDirectOnly(after))
        return CommandType::Direct;

    return type;
}

static std::vector<BarrierPlan> BuildBarriers(CommandType cmdType, const RenderPass& pass, 
    std::unordered_map<uint32_t, ResourceStateTracker>& resStateTracker)
{
    std::vector<BarrierPlan> barriers;

    for (auto& use : pass.writes)
    {
        auto& state = resStateTracker[use.handle.id].state;
        auto desired = AccessToState(cmdType, use.access);

        if (state != desired)
        {
            auto type = ResolveCommandType(cmdType, state, desired);
            barriers.push_back({ use.handle, type, state, desired });
            state = desired;
        }
    }

    return barriers;
}

static void BuildDependents(std::vector<CompiledTask>& tasks)
{
    std::unordered_map<TaskHandle, size_t> indexMap;

    for (size_t i = 0; i < tasks.size(); ++i)
        indexMap[tasks[i].handle] = i;

    for (const auto& task : tasks)
    {
        for (const auto& dep : task.task.dependencies)
        {
            auto it = indexMap.find(dep);
            if (it == indexMap.end()) continue;

            tasks[it->second].dependents.push_back(task.handle);
        }
    }
}

struct PassNode
{
    int index;
    std::vector<int> dependencies;
    int indegree{ 0 };
};

std::vector<PassNode> RenderGraph::BuildDependencyGraph()
{
    const int n = (int)m_passes.size();
    std::vector<PassNode> nodes(n);

    // 1. pass 복사
    for (int i = 0; i < n; ++i)
        nodes[i].index = i;

    auto getKey = [](const RGHandle& res) { return res.id; };

    struct ResourceUsage // 2. resource usage 수집 (WRITE / READ 분리)
    {
        std::vector<std::pair<int, RGAccess>> writers;
        std::vector<std::pair<int, RGAccess>> readers;
    };
    std::unordered_map<uint64_t, ResourceUsage> usageMap;

    for (int i = 0; i < n; ++i)
    {
        auto& pass = m_passes[i];
        for (auto& w : pass.writes)
        {
            auto key = getKey(w.handle);
            usageMap[key].writers.push_back({ i, w.access });
        }

        for (auto& r : pass.reads)
        {
            auto key = getKey(r.handle);
            usageMap[key].readers.push_back({ i, r.access });
        }
    }

    for (auto& [key, usage] : usageMap)
    {
        const auto& writers = usage.writers;
        const auto& readers = usage.readers;

        for (auto& [r, rAccess] : readers)
        {
            for (auto& [w, wAccess] : writers)
            {
                if (w != r && rAccess == wAccess)
                    nodes[r].dependencies.push_back(w); // w는 이전 r는 이후 ResourceState가 동일할때 넣는다.
            }
        }
    }

    // 4. indegree 계산
    for (int i = 0; i < n; ++i)
    {
        for (int dep : nodes[i].dependencies)
            nodes[i].indegree++;
    }

    return nodes;
}

std::vector<int> RenderGraph::TopologicalSort(const std::vector<PassNode>& graph)
{
    const int n = (int)graph.size();

    std::vector<int> indegree(n);
    
    for (int i = 0; i < n; ++i) // indegree 복사
        indegree[i] = graph[i].indegree;

    std::queue<int> q;
    std::vector<int> result;
    result.reserve(n);

    // 1. indegree 0부터 시작
    for (int i = 0; i < n; ++i)
    {
        if (indegree[i] == 0)
            q.push(i);
    }

    // 2. BFS Kahn algorithm
    while (!q.empty())
    {
        int cur = q.front();
        q.pop();

        result.push_back(cur);

        for (int i = 0; i < n; ++i)
        {
            for (int dep : graph[i].dependencies) // i가 cur를 dependency로 가지고 있다면
            {
                if (dep == cur)
                {
                    indegree[i]--;

                    if (indegree[i] == 0)
                        q.push(i);
                }
            }
        }
    }

    return result;
}

static vector<Task> CreateBarrierTask(const std::vector<BarrierPlan>& barriers, TaskScheduler& scheduler)
{
    vector<Task> barrierTasks;
    for (auto& barrier : barriers)
    {
        Task barrierTask{};
        barrierTask.type = barrier.cmdType;
        barrierTask.gpuExecute = [barrier](CommandList& cmd, TaskContext& ctx) {
            auto res = ctx.GetResource<ComPtr<ID3D12Resource>>(barrier.handle).Get();
            CommandUtils::Transition(cmd, res, barrier.before, barrier.after);
            };

        barrierTasks.push_back(barrierTask);
    }

    return barrierTasks;
}

std::vector<CompiledTask> RenderGraph::Compile(TaskScheduler& scheduler)
{
    std::unordered_map<uint32_t, ResourceStateTracker> statesTracker;
    std::unordered_map<uint32_t, TaskHandle> lastWriter;
    std::vector<CompiledTask> compiledTasks;

    auto passNode = BuildDependencyGraph();
    auto sortedPass = TopologicalSort(passNode);
    for(auto i : sortedPass)
    {
        auto& pass = m_passes[i];

        Task task{};
        task.type = pass.type;
        
        for (auto& use : pass.reads)
        {
            if (lastWriter.contains(use.handle.id))
                task.dependencies.push_back(lastWriter[use.handle.id]);
        }

        for (auto& use : pass.writes)
        {
            if (lastWriter.contains(use.handle.id))
                task.dependencies.push_back(lastWriter[use.handle.id]);
        }

        auto barriers = BuildBarriers(task.type, pass, statesTracker); //barrier가 필요하면 task를 만든다.
        if (!barriers.empty())
        {
            auto taskDependencies = task.dependencies;
            task.dependencies.clear();

            auto barrierTasks = CreateBarrierTask(barriers, scheduler);
            for (auto& barrierTask : barrierTasks)
            {
                barrierTask.dependencies = taskDependencies;

                TaskHandle barrierHandle = scheduler.AllocateHandle();
                compiledTasks.push_back({ barrierHandle, std::move(barrierTask) });
                task.dependencies.push_back(barrierHandle);
            }
        }

        task.gpuExecute = pass.gpuExecute;
        task.cpuExecute = pass.cpuExecute;

        TaskHandle handle = scheduler.AllocateHandle();
        compiledTasks.push_back({ handle, std::move(task) });
        for (auto& use : pass.writes)
            lastWriter[use.handle.id] = handle;
    }

    BuildDependents(compiledTasks);

    return compiledTasks;
}