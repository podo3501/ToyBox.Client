#pragma once
#include "RGTypes.h"
#include "Command/CommandType.h"

struct TaskContext;
class CommandList;

struct ResourceStateTrackerV
{
    D3D12_RESOURCE_STATES state{ D3D12_RESOURCE_STATE_COMMON };
};

enum class AccessType
{
    Read,
    Write
};

struct RGUsageV
{
    RGHandle handle;
    AccessType access;
    RGAccess state;
};

struct BarrierPlanV
{
    RGHandle handle;

    D3D12_RESOURCE_STATES before;
    D3D12_RESOURCE_STATES after;
};

struct RenderPassV
{
    std::string name;
    CommandType type;

    std::vector<RGUsageV> usages;

    std::function<void(CommandList&, TaskContext&)> gpuExecute;
    std::function<void(TaskContext&)> cpuExecute;

    void Read(RGHandle h, RGAccess s) 
    { 
        Assert(!HasUsage(h)); //같은 패스에서 리소스를 중복해서 쓰면 안된다.
        usages.push_back({ h, AccessType::Read, s }); 
    }

    void Write(RGHandle h, RGAccess s) 
    { 
        Assert(!HasUsage(h)); //같은 패스에서 리소스를 중복해서 쓰면 안된다.
        usages.push_back({ h, AccessType::Write, s }); 
    }

private:
    bool HasUsage(RGHandle h) const
    {
        for (const auto& u : usages)
        {
            if (u.handle.id == h.id)
                return true;
        }
        return false;
    }
};