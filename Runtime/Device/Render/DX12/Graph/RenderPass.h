#pragma once
#include "RGTypes.h"
#include "Command/CommandType.h"

struct TaskContext;
class CommandList;

enum class AccessType
{
    Read,
    Write
};

struct RGUsage
{
    RGResourceID resID;
    AccessType access;
    RGAccess state;
};

struct BarrierPlan
{
    RGResourceID resID;

    D3D12_RESOURCE_STATES before;
    D3D12_RESOURCE_STATES after;
};

using BarrierGroups = std::unordered_map<CommandType, std::vector<BarrierPlan>>;

struct RenderPass
{
    std::string name;
    CommandType type;
    CommandType waitFence{ CommandType::None }; //command type의 fence값을 넣어서 그 fence 이후에 실행되도록 함.

    std::vector<RGUsage> usages;

    std::function<void(CommandList&, TaskContext&)> gpuExecute;
    std::function<void(TaskContext&)> cpuExecute;

    void Read(RGResourceID resID, RGAccess s) 
    { 
        Assert(!HasUsage(resID)); //같은 패스에서 리소스를 중복해서 쓰면 안된다.
        usages.push_back({ resID, AccessType::Read, s });
    }

    void Write(RGResourceID resID, RGAccess s) 
    { 
        Assert(!HasUsage(resID)); //같은 패스에서 리소스를 중복해서 쓰면 안된다.
        usages.push_back({ resID, AccessType::Write, s });
    }

    void WaitFence(CommandType fenceType)
    {
        Assert(type == CommandType::None); //cpu pass에서만 사용됨.
        waitFence = fenceType;
    }

private:
    bool HasUsage(RGResourceID resID) const
    {
        for (const auto& u : usages)
        {
            if (u.resID == resID)
                return true;
        }
        return false;
    }
};