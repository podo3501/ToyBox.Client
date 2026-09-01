#pragma once
#include "RenderPass.h"
#include "RenderGraphDefinitions.h"
#include "Task.h"

class RenderGraph
{
public:
    ~RenderGraph();
    RenderGraph();
    void ImportResource(RGResourceID resID, RGAccess access); //초기상태는 이래야 함.
    void ExportResource(RGResourceID resID, RGAccess access); //끝날때는 이렇게 끝나게끔. (다른 상태면 상태변환 함)

    RenderPass& AddGraphicsPass(std::string name);
    RenderPass& AddCopyPass(std::string name);
    RenderPass& AddComputePass(std::string name);
    RenderPass& AddCpuPass(std::string name);

    std::vector<CompiledTask> Compile();
    void Reset();

private:
    struct PlannedBarrier 
    {
        BarrierGroups groups;
        LocalTaskID generatedTaskId{ InvalidLocalTaskID }; // 실제 테스크가 생성되면 여기에 기록됨
    };
    using BarrierMap = std::unordered_map<PassIndex, std::vector<std::shared_ptr<PlannedBarrier>>>;

    struct ExportResourceState
    {
        RGResourceID resID;
        RGAccess access;
    };

    RenderPass& AddPass(std::string name, CommandType type);
    void ValidateGraph();
    void BuildExportPass();
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

    std::vector<RenderPass> m_passes;
    std::unordered_map<RGResourceID, ResourceStateTracker> m_statesTracker;
    std::vector<ExportResourceState> m_exportResources;
    LocalTaskID m_localTaskID{ 0 }; //그래프에서 발급하는 임시 테스크 ID
};