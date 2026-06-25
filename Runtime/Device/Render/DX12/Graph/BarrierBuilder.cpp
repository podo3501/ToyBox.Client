#include "pch.h"
#include "BarrierBuilder.h"
#include "RenderGraphDefinitions.h"
#include "Command/CommandList.h"
#include "Command/CommandListHelpers.h"
#include "Core/D3D12Conversions.h"

static D3D12_RESOURCE_STATES AccessToState(CommandType cmdType, RGAccess access)
{
    if (access == RGAccess::CopyDest)
    {
        if (cmdType == CommandType::Copy)
            return D3D12_RESOURCE_STATE_COMMON; //copy queue 일때에는 common에서 처리하기 때문이다.

        return D3D12_RESOURCE_STATE_COPY_DEST;
    }

    return ToD3D12(access);
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

BarrierGroups BuildBarriers(
    CommandType cmdType,
    const RenderPassV& pass,
    std::unordered_map<RGResourceID, ResourceStateTrackerV>& stateTracker,
    PassIndex passIndex)
{
    BarrierGroups groups;

    for (auto& usage : pass.usages)
    {
        auto& tracker = stateTracker[usage.resID];
        auto desired = AccessToState(cmdType, usage.state);

        if (tracker.state != desired)
        {
            auto barrierType = ResolveCommandType(cmdType, tracker.state, desired);
            groups[barrierType].push_back({ usage.resID, tracker.state, desired });

            tracker.state = desired;
            tracker.lastUpdatedPass = passIndex;
        }
    }

    return groups;
}

Task CreateBarrierTask(CommandType type, const std::vector<BarrierPlanV>& barriers)
{
    Task task{};
    task.passName = "Barrier";
    task.type = type;
    task.gpuExecute = [barriers](CommandList& cmd, TaskContext& ctx) {
        std::vector<D3D12_RESOURCE_BARRIER> barrierBatch;
        barrierBatch.reserve(barriers.size());

        for (auto& barrier : barriers)
        {
            auto& res = ctx.GetResource(RGHandle(barrier.resID)); //?!? 이거 나중에 RGHandle 타입 없어지면 수정해야함.

            barrierBatch.push_back(
                CommandUtils::CreateTransitionBarrier(res, barrier.before, barrier.after));
        }

        CommandUtils::Transition(cmd, barrierBatch);
        };

    return task;
}
