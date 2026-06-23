#include "pch.h"
#include "RenderGraphV.h"
#include "BarrierBuilder.h"

RenderGraphV::~RenderGraphV() = default;

RGHandle RenderGraphV::CreateRGHandle()
{
    RGHandle handle{ m_nextId++ };
    return handle;
}

RenderPassV& RenderGraphV::AddGraphicsPass(std::string n) { return AddPass(std::move(n), CommandType::Direct);}
RenderPassV& RenderGraphV::AddCopyPass(std::string n) { return AddPass(std::move(n), CommandType::Copy); }
RenderPassV& RenderGraphV::AddComputePass(std::string n) { return AddPass(std::move(n), CommandType::Compute); }
RenderPassV& RenderGraphV::AddCpuPass(std::string n) { return AddPass(std::move(n), CommandType::None); }

RenderPassV& RenderGraphV::AddPass(std::string name, CommandType type)
{
    m_passes.emplace_back();

    auto& pass = m_passes.back();
    pass.name = std::move(name);
    pass.type = type;

    return pass;
}

struct PassNodeV
{
    int index;
    std::vector<int> dependencies;
    int indegree{ 0 };
};

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

std::vector<CompiledTask> RenderGraphV::Compile()
{
    std::vector<CompiledTask> compiledTasks;

    auto passNodes = BuildDependencyGraph();
    auto sortedPass = TopologicalSort(passNodes);

    std::unordered_map<int, uint32_t> passToTaskId;

    for (auto passIndex : sortedPass)
    {
        auto& pass = m_passes[passIndex];

        std::vector<uint32_t> dependencies;
        for (auto depPass : passNodes[passIndex].dependencies)
        {
            auto it = passToTaskId.find(depPass);
            if (it != passToTaskId.end())
                dependencies.push_back(it->second);
        }

        Task task{};
        task.passName = pass.name;
        task.type = pass.type;

        auto barrierGroups = BuildBarriers(task.type, pass, m_statesTracker);
        for (auto& [type, barriers] : barrierGroups)
        {
            auto barrierTask = CreateBarrierTask(type, barriers);
            auto barrierId = CreateLocalTaskID();

            compiledTasks.push_back({ barrierId, std::move(barrierTask), dependencies });

            dependencies.clear();
            dependencies.push_back(barrierId);
        }

        if (!pass.cpuExecute && !pass.gpuExecute)
            continue;

        task.cpuExecute = pass.cpuExecute;
        task.gpuExecute = pass.gpuExecute;

        auto taskId = CreateLocalTaskID();

        compiledTasks.push_back({ taskId, std::move(task), dependencies });

        passToTaskId[passIndex] = taskId;
    }
    BuildDependents(compiledTasks);

    return compiledTasks;
}

std::vector<PassNodeV> RenderGraphV::BuildDependencyGraph()
{
    const int passCount = static_cast<int>(m_passes.size());

    std::vector<PassNodeV> nodes(passCount);

    for (int i = 0; i < passCount; ++i)
        nodes[i].index = i;

    std::unordered_map<uint32_t, int> lastWriter; // resource -> 마지막 writer pass

    for (int passIndex = 0; passIndex < passCount; ++passIndex)
    {
        auto& pass = m_passes[passIndex];

        for (auto& usage : pass.usages)
        {
            switch (usage.access)
            {
            case AccessType::Read:
            {
                auto it = lastWriter.find(usage.handle.id);
                if (it != lastWriter.end())
                    nodes[passIndex].dependencies.push_back(it->second);
                break;
            }

            case AccessType::Write:
            {
                auto it = lastWriter.find(usage.handle.id);
                if (it != lastWriter.end())
                    nodes[passIndex].dependencies.push_back(it->second);
                lastWriter[usage.handle.id] = passIndex;
                break;
            }
            }
        }
    }

    for (auto& node : nodes)
    {
        std::sort(node.dependencies.begin(), node.dependencies.end());

        node.dependencies.erase(
            std::unique(
                node.dependencies.begin(),
                node.dependencies.end()),
            node.dependencies.end());

        node.indegree =
            static_cast<int>(node.dependencies.size());
    }

    return nodes;
}

std::vector<int> RenderGraphV::TopologicalSort(const std::vector<PassNodeV>& graph)
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

uint32_t RenderGraphV::CreateLocalTaskID()
{
    return m_nextTaskId++;
}

