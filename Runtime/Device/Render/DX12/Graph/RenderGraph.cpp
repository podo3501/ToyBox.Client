#include "pch.h"
#include "RenderGraph.h"
#include "BarrierBuilder.h"
#include "Core/D3D12Conversions.h"
#include "RenderGraphUtils.h"
#include <unordered_set>

std::atomic<RGResourceID> RenderGraph::s_resourceID{ 1 };

RenderGraph::~RenderGraph() = default;
RGResourceID RenderGraph::CreateRGResourceID() noexcept
{
    return s_resourceID.fetch_add(1);
}

void RenderGraph::ImportResource(RGResourceID resID, RGAccess access)
{
    m_statesTracker[resID].state = ToD3D12(access);
}

void RenderGraph::ExportResource(RGResourceID resID, RGAccess access)
{
    Assert(m_statesTracker.contains(resID)); // 초기상태가 있어야 함.
    m_exportResources.emplace_back(resID, access);
}

RenderPass& RenderGraph::AddGraphicsPass(std::string n) { return AddPass(std::move(n), CommandType::Direct);}
RenderPass& RenderGraph::AddCopyPass(std::string n) { return AddPass(std::move(n), CommandType::Copy); }
RenderPass& RenderGraph::AddComputePass(std::string n) { return AddPass(std::move(n), CommandType::Compute); }
RenderPass& RenderGraph::AddCpuPass(std::string n) { return AddPass(std::move(n), CommandType::None); }

RenderPass& RenderGraph::AddPass(std::string name, CommandType type)
{
    m_passes.emplace_back();

    auto& pass = m_passes.back();
    pass.name = std::move(name);
    pass.type = type;

    return pass;
}

std::vector<CompiledTask> RenderGraph::Compile()
{
    ValidateGraph();

    BuildExportPass();
    auto passNodes = BuildDependencyGraph(); //dependency 만들기
    auto sortedPass = TopologicalSort(passNodes); //만든 걸로 node 정렬
    auto barrierMap = PlanBarriers(sortedPass); // 배리어 '위치'와 '실물 정보' 완벽히 선점하기(베리어 먼저 만들어보기)

    auto tasks = BuildCompiledTasks(passNodes, sortedPass, barrierMap);
    BuildDependents(tasks);

    return tasks;
}

void RenderGraph::BuildExportPass()
{
    if (m_exportResources.empty())
        return;

    auto& pass = AddCpuPass("__Export_Internal");
    for (auto& [resID, access] : m_exportResources)
        pass.Write(resID, access);
}

std::vector<CompiledTask> RenderGraph::BuildCompiledTasks(
    const std::vector<PassNodeV>& passNodes,
    const std::vector<PassIndex>& sortedPass,
    const BarrierMap& passToBarriersMap)
{
    std::vector<CompiledTask> tasks;
    tasks.reserve(sortedPass.size() * 2); //여유를 줘서.
    std::unordered_map<PassIndex, LocalTaskID> passToTaskId;
    passToTaskId.reserve(sortedPass.size());

    for (PassIndex passIndex : sortedPass)
    {
        auto& pass = m_passes[passIndex];

        // 부모 패스 의존성 수집
        std::vector<LocalTaskID> baseDependencies;

        for (PassIndex depPass : passNodes[passIndex].dependencies)
        {
            auto it = passToTaskId.find(depPass);
            if (it != passToTaskId.end())
                baseDependencies.push_back(it->second);
        }

        std::vector<LocalTaskID> currentPassDependencies =
            BuildBarrierTasks(passIndex, baseDependencies, passToBarriersMap, tasks);

        if (!pass.cpuExecute && !pass.gpuExecute)
            continue;

        Task task{};
        task.passName = pass.name;
        task.type = pass.type;
        task.cpuExecute = pass.cpuExecute;
        task.gpuExecute = pass.gpuExecute;

        LocalTaskID taskId = CreateLocalTaskID();
        tasks.push_back({ taskId, std::move(task), currentPassDependencies });

        passToTaskId[passIndex] = taskId;
    }

    return tasks;
}

std::vector<LocalTaskID> RenderGraph::BuildBarrierTasks(
    PassIndex passIndex,
    const std::vector<LocalTaskID>& baseDependencies,
    const BarrierMap& passToBarriersMap,
    std::vector<CompiledTask>& outTasks)
{
    auto barrierIt = passToBarriersMap.find(passIndex);
    if (barrierIt == passToBarriersMap.end() || barrierIt->second.empty())
        return baseDependencies; // 배리어가 없으면 부모 의존성을 그대로 반환

    std::vector<LocalTaskID> currentPassDependencies;
    currentPassDependencies.reserve(barrierIt->second.size());

    for (auto& planned : barrierIt->second)
    {
        if (planned->generatedTaskId == 0) // 아직 배리어 태스크가 생성되지 않은 경우에만 새로 생성 (캐싱 로직 유지)
        {
            std::vector<LocalTaskID> barrierDependencies = baseDependencies;
            for (auto& [type, barriers] : planned->groups)
            {
                auto barrierTask = CreateBarrierTask(type, barriers);
                LocalTaskID barrierId = CreateLocalTaskID();
                outTasks.push_back({ barrierId, std::move(barrierTask), std::move(barrierDependencies) });

                barrierDependencies.clear();
                barrierDependencies.push_back(barrierId); // 체이닝: 다음 그룹 배리어는 방금 생성한 배리어 태스크에 의존하도록 설정
            }
            planned->generatedTaskId = barrierDependencies.back();
        }
        currentPassDependencies.push_back(planned->generatedTaskId);
    }
    RemoveVectorDuplicates(currentPassDependencies);

    return currentPassDependencies;
}

std::vector<PassNodeV> RenderGraph::BuildDependencyGraph()
{
    const int passCount = static_cast<int>(m_passes.size());

    std::vector<PassNodeV> nodes(passCount);

    for (int i = 0; i < passCount; ++i)
        nodes[i].index = i;

    std::unordered_map<RGResourceID, PassIndex> lastWriter; // resource -> 마지막 writer pass. waw, raw(write->read)를 하기위한 변수.
    std::unordered_map<RGResourceID, std::vector<PassIndex>> activeReaders; //war(read->write) 에 필요한 변수. war은 조금 까다롭다.

    for (PassIndex passIndex = 0; passIndex < passCount; ++passIndex)
    {
        auto& pass = m_passes[passIndex];

        for (auto& usage : pass.usages)
        {
            const auto resourceID = usage.resID;

            switch (usage.access)
            {
            case AccessType::Read:
            {
                // RAW(write->read)
                auto writerIt = lastWriter.find(resourceID);
                if (writerIt != lastWriter.end())
                {
                    int writerPass = writerIt->second;

                    nodes[passIndex].dependencies.push_back(writerPass);
                    nodes[writerPass].dependents.push_back(passIndex);
                }

                activeReaders[resourceID].push_back(passIndex);
                break;
            }

            case AccessType::Write:
            {
                // WAW(write->write)
                auto writerIt = lastWriter.find(resourceID);
                if (writerIt != lastWriter.end())
                {
                    int writerPass = writerIt->second;

                    nodes[passIndex].dependencies.push_back(writerPass);
                    nodes[writerPass].dependents.push_back(passIndex);
                }

                // WAR(read->write). 참고로 RAR은 하지 않는다.
                auto readerIt = activeReaders.find(resourceID);
                if (readerIt != activeReaders.end())
                {
                    for (int readerPass : readerIt->second)
                    {
                        nodes[passIndex].dependencies.push_back(readerPass);
                        nodes[readerPass].dependents.push_back(passIndex);
                    }

                    readerIt->second.clear();
                }

                lastWriter[resourceID] = passIndex;
                break;
            }
            }
        }
    }

    for (auto& node : nodes)
    {
        RemoveVectorDuplicates(node.dependencies); // 정방향 간선 중복 제거
        RemoveVectorDuplicates(node.dependents); // 역방향 간선 중복 제거

        node.indegree = static_cast<int>(node.dependencies.size());
    }

    return nodes;
}

void RenderGraph::ValidateGraph()
{
    std::unordered_set<RGResourceID> produced;

    for (auto& [resID, state] : m_statesTracker)
        produced.insert(resID);

    for (auto& pass : m_passes)
    {
        for (auto& usage : pass.usages)
        {
            if (usage.access == AccessType::Read)
            {
                Assert(produced.contains(usage.resID)); //read pass는 import가 있거나 write pass가 있어야 한다.(즉, 읽을게 있어야 읽지)
            }

            if (usage.access == AccessType::Write)
            {
                produced.insert(usage.resID);
            }
        }
    }
}

RenderGraph::BarrierMap RenderGraph::PlanBarriers(const std::vector<PassIndex>& sortedPass)
{
    std::unordered_map<PassIndex, std::vector<std::shared_ptr<PlannedBarrier>>> passToBarriersMap;
    std::unordered_map<RGResourceID, std::shared_ptr<PlannedBarrier>> lastResourceBarrier;

    auto tempTracker = m_statesTracker;

    for (PassIndex passIndex : sortedPass)
    {
        auto& pass = m_passes[passIndex];
        BarrierGroups barrierGroups = BuildBarriers(pass.type, pass, tempTracker, passIndex);

        if (!barrierGroups.empty())
        {
            auto planned = std::make_shared<PlannedBarrier>();
            planned->groups = std::move(barrierGroups);

            passToBarriersMap[passIndex].push_back(planned);
            for (const auto& usage : pass.usages)
            {
                if (tempTracker[usage.resID].lastUpdatedPass == passIndex) //이번 패스에서 실제로 트랜지션 배리어가 생성된 "자원"만 Fork용 이정표로 등록.
                    lastResourceBarrier[usage.resID] = planned;
            }
        }
        else
        {
            // (Fork 핵심) 배리어가 새로 안 만들어졌다면, 형제 pass가 이미 선점해둔 배리어가 있는지 공유 엮기
            for (const auto& usage : pass.usages)
            {
                if (usage.access == AccessType::Read && lastResourceBarrier.contains(usage.resID))
                    passToBarriersMap[passIndex].push_back(lastResourceBarrier[usage.resID]);
            }
        }
    }

    m_statesTracker = std::move(tempTracker); // 시뮬레이션이 끝난 최종 자원 상태를 전역 트래커에 동기화
    return passToBarriersMap;
}

LocalTaskID RenderGraph::CreateLocalTaskID()
{
    return m_localTaskID++;
}

