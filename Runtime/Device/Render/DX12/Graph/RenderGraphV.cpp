#include "pch.h"
#include "RenderGraphV.h"
#include "BarrierBuilder.h"
#include "Core/D3D12Conversions.h"
#include <unordered_set>

RenderGraphV::~RenderGraphV() = default;

RGHandle RenderGraphV::CreateRGHandle()
{
    RGHandle handle{ m_nextId++ };
    return handle;
}

void RenderGraphV::ImportResource(RGHandle h, RGAccess access)
{
    m_statesTracker[h.id].state = ToD3D12(access);
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
    std::vector<int> dependencies; // 내 앞에 실행되어야 하는 패스들 (정방향)
    std::vector<int> dependents; // 내 뒤에 실행되어야 하는 패스들 (역방향) 나중에 barrier를 만들고 나서 다시 역방향을 만든다. barrier를 만들때 역방향을 연결해 줄 수도 있지만, 다시 만들어도 비교적 비용이 싸고 유지보수가 더 쉽기 때문이다.
    int indegree{ 0 };
};

static void BuildDependents(std::vector<CompiledTask>& tasks)
{
    std::unordered_map<uint32_t, size_t> indexMap;
    indexMap.reserve(tasks.size());

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
    ValidateGraph();

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

    std::unordered_map<uint32_t, int> lastWriter; // resource -> 마지막 writer pass. waw, raw(write->read)를 하기위한 변수.
    std::unordered_map<uint32_t, std::vector<int>> activeReaders; //war(read->write) 에 필요한 변수. war은 조금 까다롭다.

    for (int passIndex = 0; passIndex < passCount; ++passIndex)
    {
        auto& pass = m_passes[passIndex];

        for (auto& usage : pass.usages)
        {
            const auto resourceId = usage.handle.id;

            switch (usage.access)
            {
            case AccessType::Read:
            {
                // RAW(write->read)
                auto writerIt = lastWriter.find(resourceId);
                if (writerIt != lastWriter.end())
                {
                    int writerPass = writerIt->second;

                    nodes[passIndex].dependencies.push_back(writerPass);
                    nodes[writerPass].dependents.push_back(passIndex);
                }

                activeReaders[resourceId].push_back(passIndex);
                break;
            }

            case AccessType::Write:
            {
                // WAW(write->write)
                auto writerIt = lastWriter.find(resourceId);
                if (writerIt != lastWriter.end())
                {
                    int writerPass = writerIt->second;

                    nodes[passIndex].dependencies.push_back(writerPass);
                    nodes[writerPass].dependents.push_back(passIndex);
                }

                // WAR(read->write). 참고로 RAR은 하지 않는다.
                auto readerIt = activeReaders.find(resourceId);
                if (readerIt != activeReaders.end())
                {
                    for (int readerPass : readerIt->second)
                    {
                        nodes[passIndex].dependencies.push_back(readerPass);
                        nodes[readerPass].dependents.push_back(passIndex);
                    }

                    readerIt->second.clear();
                }

                lastWriter[resourceId] = passIndex;
                break;
            }
            }
        }
    }

    for (auto& node : nodes)
    {
        // 정방향 간선 중복 제거
        std::sort(node.dependencies.begin(), node.dependencies.end());
        node.dependencies.erase(
            std::unique(node.dependencies.begin(), node.dependencies.end()), node.dependencies.end());

        // 역방향 간선 중복 제거
        std::sort(node.dependents.begin(), node.dependents.end());
        node.dependents.erase(
            std::unique(node.dependents.begin(), node.dependents.end()), node.dependents.end());

        node.indegree = static_cast<int>(node.dependencies.size());
    }

    return nodes;
}

void RenderGraphV::ValidateGraph()
{
    std::unordered_set<uint32_t> produced;

    for (auto& [id, state] : m_statesTracker)
        produced.insert(id);

    for (auto& pass : m_passes)
    {
        for (auto& usage : pass.usages)
        {
            if (usage.access == AccessType::Read)
            {
                Assert(produced.contains(usage.handle.id)); //read pass는 import가 있거나 write pass가 있어야 한다.(즉, 읽을게 있어야 읽지)
            }

            if (usage.access == AccessType::Write)
            {
                produced.insert(usage.handle.id);
            }
        }
    }
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

    while (!q.empty()) 
    {
        int cur = q.front();
        q.pop();
        result.push_back(cur);

        for (int nextPass : graph[cur].dependents) 
        {
            indegree[nextPass]--;
            if (indegree[nextPass] == 0) 
                q.push(nextPass);
        }
    }

    return result;
}

uint32_t RenderGraphV::CreateLocalTaskID()
{
    return m_nextTaskId++;
}

