#include "pch.h"
#include "RenderGraph.h"
#include "Command/CommandListHelpers.h"
#include "Command/CommandList.h"
#include "Command/CommandType.h"
#include <unordered_set>

static D3D12_RESOURCE_STATES AccessToState(RGAccess access)
{
    switch (access)
    {
    case RGAccess::SRV: return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    case RGAccess::UAV: return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
    //case RGAccess::CopySrc: return D3D12_RESOURCE_STATE_COPY_SOURCE;
    //case RGAccess::CopyDst: return D3D12_RESOURCE_STATE_COPY_DEST;
    case RGAccess::RTV: return D3D12_RESOURCE_STATE_RENDER_TARGET;
    case RGAccess::DepthWrite: return D3D12_RESOURCE_STATE_DEPTH_WRITE;
    case RGAccess::DepthRead: return D3D12_RESOURCE_STATE_DEPTH_READ;
    case RGAccess::Present: return D3D12_RESOURCE_STATE_PRESENT;
    default: return D3D12_RESOURCE_STATE_COMMON;
    }
}

static D3D12_RESOURCE_STATES AccessToState(CommandType cmdType, RGAccess access)
{
    if (access == RGAccess::CopyDest)
    {
        if (cmdType == CommandType::Copy) 
            return D3D12_RESOURCE_STATE_COMMON; //copy queue 일때에는 common에서 처리하기 때문이다.

        return D3D12_RESOURCE_STATE_COPY_DEST;
    }

    return AccessToState(access);
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

static void BuildDependents(std::vector<CompiledTask>& tasks)
{
    std::unordered_map<uint32_t, size_t> indexMap;

    for (size_t i = 0; i < tasks.size(); ++i)
        indexMap[tasks[i].localId] = i;

    for (const auto& task : tasks)
    {
        for (const auto& dep : task.dependencies)
        {
            auto it = indexMap.find(dep);
            if (it == indexMap.end()) continue;

            tasks[it->second].dependents.push_back(task.localId);
        }
    }
}

struct PassNode
{
    int index;
    std::vector<int> dependencies;
    int indegree{ 0 };
};

int RenderGraph::FindPassIndex(const std::string& name)
{
    for (int i = 0; i < m_passes.size(); ++i)
    {
        if (m_passes[i].name == name)
            return i;
    }
    return -1;
}

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

    // 3. dependsOn 으로 강제로 순서 만들기.
    for (int i = 0; i < n; ++i)
    {
        auto& pass = m_passes[i];

        for (auto& depName : pass.dependsOn)
        {
            int depIndex = FindPassIndex(depName);
            if (depIndex < 0) continue;

            nodes[i].dependencies.push_back(depIndex);
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

using BarrierGroups = std::unordered_map<CommandType, std::vector<BarrierPlan>>;

static BarrierGroups BuildBarriers(
    CommandType cmdType, 
    const RenderPass& pass,
    std::unordered_map<uint32_t, 
    ResourceStateTracker>& resStateTracker)
{
    BarrierGroups groups;

    for (auto& use : pass.writes)
    {
        auto& state = resStateTracker[use.handle.id].state;
        auto desired = AccessToState(cmdType, use.access);

        if (state != desired)
        {
            auto type = ResolveCommandType(cmdType, state, desired);
            groups[type].push_back({ use.handle, state, desired });

            state = desired;
        }
    }

    return groups;
}

static Task CreateBarrierTask(CommandType type, const std::vector<BarrierPlan>& barriers)
{
    Task task{};
    task.passName = "barrier";
    task.type = type;
    task.gpuExecute = [barriers](CommandList& cmd, TaskContext& ctx) {
        std::vector<D3D12_RESOURCE_BARRIER> nativeBarriers;
        nativeBarriers.reserve(barriers.size());

        for (auto& barrier : barriers)
        {
            auto& res = ctx.GetResource(barrier.handle);
                
            nativeBarriers.push_back(
                CommandUtils::CreateTransitionBarrier(res, barrier.before, barrier.after));

            //DX_LOG("[Barrier] handle={} {} -> {}", barrier.handle.id, (uint32_t)barrier.before, (uint32_t)barrier.after);
        }

        //DX_LOG("[BarrierBatch] count={}", nativeBarriers.size()); //?!? 이렇게 로그를 찍지만 tdd로 테스트를 해서 값이 제대로 나오는지 테스트 하는게 정석이다. 언제 하게 될지는 미정이다.
        CommandUtils::Transition(cmd, nativeBarriers);
        };

    return task;
}

std::vector<CompiledTask> RenderGraph::Compile()
{
    std::unordered_map<uint32_t, uint32_t> lastWriter;
    std::vector<CompiledTask> compiledTasks;

    auto passNode = BuildDependencyGraph();
    auto sortedPass = TopologicalSort(passNode);
    for(auto i : sortedPass)
    {
        auto& pass = m_passes[i];

        Task task{};
        task.passName = pass.name;
        task.type = pass.type;
        vector<uint32_t> dependencies;
        
        for (auto& use : pass.reads)
        {
            if (lastWriter.contains(use.handle.id))
                dependencies.push_back(lastWriter[use.handle.id]);
        }

        for (auto& use : pass.writes)
        {
            if (lastWriter.contains(use.handle.id))
                dependencies.push_back(lastWriter[use.handle.id]);
        }

        auto barrierGroups = BuildBarriers(task.type, pass, m_statesTracker);
        for (auto& [type, barriers] : barrierGroups)
        {
            auto barrierTask = CreateBarrierTask(type, barriers);
            auto barrierID = CreateLocalTaskID();

            compiledTasks.push_back({ barrierID, std::move(barrierTask), dependencies });

            dependencies.clear();
            dependencies.push_back(barrierID);
        }

        if (!pass.cpuExecute && !pass.gpuExecute)
            continue; // barrier를 생성하기 위해서 아무것도 없는 task를 만들때가 있다. 생성되고 나면 task로 만들지는 않게 한다.

        task.gpuExecute = pass.gpuExecute;
        task.cpuExecute = pass.cpuExecute;

        auto taskID = CreateLocalTaskID();
        compiledTasks.push_back({ taskID, std::move(task), dependencies });
        for (auto& use : pass.writes)
            lastWriter[use.handle.id] = taskID;
    }
    BuildDependents(compiledTasks);

    return compiledTasks;
}

RGHandle RenderGraph::CreateRGHandle()
{
    RGHandle handle{ m_nextId++ };
    return handle;
}

uint32_t RenderGraph::CreateLocalTaskID()
{
    return m_nextTaskId++;
}

void RenderGraph::ImportResource(RGHandle h, RGAccess access)
{
    m_statesTracker[h.id].state = AccessToState(access);
}

void RenderGraph::Execute(CommandList& cmd, const vector<CompiledTask>& compiledTasks, TaskContext& ctx)
{
    for (auto& compiled : compiledTasks)
    {
        auto& task = compiled.task;
        if (task.type == CommandType::None) // CPU TASK
        {
            if (!task.cpuExecute) continue;
            task.cpuExecute(ctx);
            continue;
        }

        if (!task.gpuExecute) continue;
        task.gpuExecute(cmd, ctx);
    }
}

