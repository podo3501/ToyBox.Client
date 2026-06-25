#pragma once
#include "RenderPassV.h"
#include "Task.h"

struct ResourceStateTrackerV;

BarrierGroups BuildBarriers(
    CommandType cmdType,
    const RenderPassV& pass,
    std::unordered_map<RGResourceID, ResourceStateTrackerV>& resStateTracker,
    PassIndex passIndex);
Task CreateBarrierTask(CommandType type, const std::vector<BarrierPlanV>& barriers);
