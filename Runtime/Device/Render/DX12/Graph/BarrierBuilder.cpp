#include "pch.h"
#include "BarrierBuilder.h"
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
    std::unordered_map<uint32_t, ResourceStateTrackerV>& stateTracker)
{
    BarrierGroups groups;

    for (auto& usage : pass.usages)
    {
        auto& state = stateTracker[usage.handle.id].state;
        auto desired = AccessToState(cmdType, usage.state);

        if (state != desired)
        {
            auto barrierType = ResolveCommandType(cmdType, state, desired);
            groups[barrierType].push_back({ usage.handle, state, desired });

            state = desired;
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
            auto& res = ctx.GetResource(barrier.handle);

            barrierBatch.push_back(
                CommandUtils::CreateTransitionBarrier(res, barrier.before, barrier.after));
        }

        CommandUtils::Transition(cmd, barrierBatch);
        };

    return task;
}
