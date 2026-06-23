#pragma once
#include "RenderPassV.h"
#include "Task.h"

using BarrierGroups = std::unordered_map<CommandType, std::vector<BarrierPlanV>>;
BarrierGroups BuildBarriers(
    CommandType cmdType,
    const RenderPassV& pass,
    std::unordered_map<uint32_t, ResourceStateTrackerV>& resStateTracker);
Task CreateBarrierTask(CommandType type, const std::vector<BarrierPlanV>& barriers);
