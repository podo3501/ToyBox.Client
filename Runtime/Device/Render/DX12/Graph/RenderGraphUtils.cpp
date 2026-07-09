#include "pch.h"
#include "RenderGraphUtils.h"
#include "Task.h"

std::vector<PassIndex> TopologicalSort(const std::vector<PassNodeV>& graph)
{
    const int n = static_cast<int>(graph.size());

    std::vector<int> indegree(n);

    for (int i = 0; i < n; ++i) // indegree 복사
        indegree[i] = graph[i].indegree;

    std::queue<PassIndex> q;
    std::vector<PassIndex> result;
    result.reserve(n);

    // 1. indegree 0부터 시작
    for (PassIndex i = 0; i < n; ++i)
    {
        if (indegree[i] == 0)
            q.push(i);
    }

    while (!q.empty())
    {
        PassIndex cur = q.front();
        q.pop();
        result.push_back(cur);

        for (PassIndex nextPass : graph[cur].dependents)
        {
            indegree[nextPass]--;
            if (indegree[nextPass] == 0)
                q.push(nextPass);
        }
    }

    Assert(result.size() == graph.size()); // Dependency graph contains cycle.

    return result;
}

void BuildDependents(std::vector<CompiledTask>& tasks)
{
    std::unordered_map<LocalTaskID, size_t> indexMap;
    indexMap.reserve(tasks.size());

    for (size_t i = 0; i < tasks.size(); ++i)
        indexMap[tasks[i].localId] = i;

    for (const auto& task : tasks)
    {
        for (const auto& dep : task.dependencies)
        {
            auto it = indexMap.find(dep);
            Assert(it != indexMap.end()); //디펜던시가 존재하는데 task가 없다.

            tasks[it->second].dependents.push_back(task.localId);
        }
    }
}