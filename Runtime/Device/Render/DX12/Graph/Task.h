#pragma once
#include <cstdint>
#include <vector>
#include <functional>
#include <variant>
#include <utility>
#include <memory>
#include "d3dx12.h"
#include "Core/RenderData.h"
#include "Command/CommandType.h"
#include "Resource/Resource.h"
#include "TaskHandle.h"
#include "RGTypes.h"
#include "GameClient/Graphics/RenderData/DirectionalLightData.h"
#include "GameClient/Graphics/RenderData/CameraData.h"

class CommandList;

using Microsoft::WRL::ComPtr;

struct ResourceContext
{
    std::unordered_map<uint32_t, Resource> resources;

    void Set(RGHandle h, const Resource& resource) { resources[h.id] = resource; }
    void Set(RGHandle h, Resource&& resource) { resources[h.id] = std::move(resource); }

    Resource& Get(RGHandle h)
    {
        auto it = resources.find(h.id);
        Assert(it != resources.end());

        return it->second;
    }

    const Resource& Get(RGHandle h) const
    {
        auto it = resources.find(h.id);
        Assert(it != resources.end());

        return it->second;
    }
};

struct FrameData
{
    DirectionalLightData light;
    CameraData camera;
};

struct TaskContext
{
    DrawPacket drawPacket;
    FrameData frame;
    shared_ptr<ResourceContext> resources; //중요한 리소스. 공유됨.

    void SetResource(RGHandle h, const Resource& resource) const { resources->Set(h, resource); }
    void SetResource(RGHandle h, Resource&& resource) const { resources->Set(h, std::move(resource)); }
    Resource& GetResource(RGHandle h) const { return resources->Get(h); }
};

struct Task
{
    std::string passName;
    CommandType type;
    std::vector<TaskHandle> dependencies; //앞에 Task에 의존하는지. Task의 시작지점을 알게 해 준다.
    std::function<void(CommandList&, TaskContext&)> gpuExecute{ nullptr };
    std::function<void(TaskContext&)> cpuExecute{ nullptr };
};

struct CompiledTask //RenderGraph에서 pass를 가지고 계산해서 tasks로 만든 결과물.
{
    uint32_t localId{ 0 };
    Task task;
    std::vector<uint32_t> dependencies;
    std::vector<uint32_t> dependents;
};