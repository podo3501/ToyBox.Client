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
    RGTexture tex;
    CommandType cmdType{ CommandType::Direct };
    D3D12_RESOURCE_STATES before;
    D3D12_RESOURCE_STATES after;
};

enum class ResourceState
{
    Undefined,

    // upload / transfer phase
    TransferWrite,
    TransferRead,

    // shader usage phase
    ShaderRead,
    ShaderWrite,

    // graphics pipeline usage
    RenderTarget,
    DepthStencil
};

enum class ResourceAccessType
{
    None,       // 의미 없음 (state transition-only pass)

    Copy,       // CopyQueue (upload/download)

    SRV,        // Shader Resource View (read-only)
    UAV,        // Unordered Access View (read/write compute)

    RTV,        // Render Target View (color output)
    DSV         // Depth Stencil View (depth output)
};

struct ResourceAccess
{
    RGTexture tex;

    ResourceState before;
    ResourceState after;

    ResourceAccessType type;
};

struct RenderPass
{
    std::string name;
    CommandType type;

    std::vector<ResourceAccess> accesses;

    std::vector<RGUsage> reads;
    std::vector<RGUsage> writes;
    std::vector<BarrierPlan> barriers;

    std::function<void(CommandList&, TaskContext&)> gpuExecute;
    std::function<void(TaskContext&)> cpuExecute;
};