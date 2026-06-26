#pragma once
#include "RenderPass.h"
#include "Task.h"

struct ResourceStateTracker;

BarrierGroups BuildBarriers(
    CommandType cmdType,
    const RenderPass& pass,
    std::unordered_map<RGResourceID, ResourceStateTracker>& resStateTracker,
    PassIndex passIndex);
Task CreateBarrierTask(CommandType type, const std::vector<BarrierPlan>& barriers);
