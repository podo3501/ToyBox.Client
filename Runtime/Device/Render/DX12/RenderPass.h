#pragma once
#include <string>
#include <vector>
#include <functional>

#include "CommandType.h"
#include "Task.h"
#include "RGTypes.h"

class CommandList;

struct BarrierPlan
{
    RGHandle handle;
    CommandType cmdType{ CommandType::Direct };
    D3D12_RESOURCE_STATES before;
    D3D12_RESOURCE_STATES after;
};

struct RenderPass
{
    std::string name;
    CommandType type;

    std::vector<RGUsage> reads;
    std::vector<RGUsage> writes;
    std::vector<BarrierPlan> barriers;

    std::function<void(CommandList&, TaskContext&)> gpuExecute;
    std::function<void(TaskContext&)> cpuExecute;
};