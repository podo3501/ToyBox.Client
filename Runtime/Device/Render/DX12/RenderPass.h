#pragma once
#include <string>
#include <vector>
#include <functional>

#include "Command/CommandType.h"
#include "Task.h"
#include "RGTypes.h"

class CommandList;

struct BarrierPlan
{
    RGHandle handle;

    D3D12_RESOURCE_STATES before;
    D3D12_RESOURCE_STATES after;
};

struct RenderPass
{
    std::string name;
    CommandType type;

    std::vector<RGUsage> reads;
    std::vector<RGUsage> writes;
    std::vector<std::string> dependsOn;
    std::vector<BarrierPlan> barriers;

    std::function<void(CommandList&, TaskContext&)> gpuExecute;
    std::function<void(TaskContext&)> cpuExecute;
};