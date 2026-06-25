#include "pch.h"
#include "RenderGraphUtils.h"

std::vector<PassIndex> TopologicalSort(const std::vector<PassNodeV>& graph)
{
    const int n = (int)graph.size();

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

    Assert(result.size() == graph.size());

    return result;
}
