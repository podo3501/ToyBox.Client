#pragma once
#include "RenderPassV.h"
#include "RenderGraphDefinitions.h"
#include "Task.h"

class RenderGraphV
{
public:
    ~RenderGraphV();
    RGResourceID CreateRGResourceID();
    void ImportResource(RGResourceID resID, RGAccess access);

    RenderPassV& AddGraphicsPass(std::string name);
    RenderPassV& AddCopyPass(std::string name);
    RenderPassV& AddComputePass(std::string name);
    RenderPassV& AddCpuPass(std::string name);

    std::vector<CompiledTask> Compile();

private:
    struct PlannedBarrier 
    {
        BarrierGroups groups;
        LocalTaskID generatedTaskId{ 0 }; // 실제 테스크가 생성되면 여기에 기록됨
    };
    using BarrierMap = std::unordered_map<PassIndex, std::vector<std::shared_ptr<PlannedBarrier>>>;

    RenderPassV& AddPass(std::string name, CommandType type);
    void ValidateGraph();
    std::vector<PassNodeV> BuildDependencyGraph();
    BarrierMap PlanBarriers(const std::vector<PassIndex>& sortedPass); //배리어 계획 단계를 담당

    std::vector<CompiledTask> BuildCompiledTasks(
        const std::vector<PassNodeV>& passNodes,
        const std::vector<PassIndex>& sortedPass,
        const BarrierMap& passToBarriersMap);

    std::vector<LocalTaskID> BuildBarrierTasks(
        PassIndex passIndex,
        const std::vector<LocalTaskID>& baseDependencies,
        const BarrierMap& passToBarriersMap,
        std::vector<CompiledTask>& outTasks);

    LocalTaskID CreateLocalTaskID();

    std::vector<RenderPassV> m_passes;
    std::unordered_map<RGResourceID, ResourceStateTrackerV> m_statesTracker;
    RGResourceID m_resID{ 1 }; //리소스 id를 발급해 주는 변수
    LocalTaskID m_localTaskID{ 1 }; //그래프에서 발급하는 임시 테스크 ID
};