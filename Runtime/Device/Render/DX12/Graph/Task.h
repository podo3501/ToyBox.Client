#pragma once
#include <cstdint>
#include <vector>
#include <functional>
#include <variant>
#include <utility>
#include <memory>
#include "d3dx12.h"
#include "Definition/RenderData.h"
#include "Command/CommandType.h"
#include "Resource/Resource.h"
#include "TaskHandle.h"
#include "RGTypes.h"
#include "GameClient/Graphics/RenderData/FrameData.h"

class CommandList;

using Microsoft::WRL::ComPtr;

struct ResourceContext
{
    ~ResourceContext() = default;
    ResourceContext() = delete;
    explicit ResourceContext(size_t capacity) : resources(capacity) {}

    void Set(RGResourceID id, const Resource& resource) { resources[ToIndex(id)] = resource; }
    void Set(RGResourceID id, Resource&& resource) { resources[ToIndex(id)] = std::move(resource); }

    Resource& Get(RGResourceID id)
    {
        auto& slot = resources[ToIndex(id)];
        Assert(slot.has_value()); // 리소스 등록하는 부분이 빠져 있을 가능성.
        return *slot;
    }

    const Resource& Get(RGResourceID id) const
    {
        auto& slot = resources[ToIndex(id)];
        Assert(slot.has_value());
        return *slot;
    }

private:
    size_t ToIndex(RGResourceID id) const
    {
        size_t idx = static_cast<size_t>(id);
        Assert(idx < resources.size()); // capacity를 잘못 넘겼을 가능성
        return idx;
    }

    std::vector<std::optional<Resource>> resources;
};

//struct ResourceContext
//{
//    std::unordered_map<uint32_t, Resource> resources;
//
//    void Set(RGResourceID id, const Resource& resource) { resources[id] = resource; }
//    void Set(RGResourceID id, Resource&& resource) { resources[id] = std::move(resource); }
//
//    Resource& Get(RGResourceID id)
//    {
//        auto it = resources.find(id);
//        Assert(it != resources.end()); //리소스 등록하는 부분이 빠져 있을 가능성. taskContext에서 리소스 등록이 안돼 있을 수 있음.
//        return it->second;
//    }
//
//    const Resource& Get(RGResourceID id) const
//    {
//        auto it = resources.find(id);
//        Assert(it != resources.end());
//        return it->second;
//    }
//};

struct TaskContext
{
    shared_ptr<ResourceContext> resources; //중요한 리소스. 공유됨.

    void SetResource(RGResourceID id, const Resource& resource) const { resources->Set(id, resource); }
    void SetResource(RGResourceID id, Resource&& resource) const { resources->Set(id, std::move(resource)); }
    Resource& GetResource(RGResourceID id) const { return resources->Get(id); }
};

struct Task
{
    std::string passName{};
    CommandType type{ CommandType::None };

    std::vector<TaskHandle> dependencies; //앞에 Task에 의존하는지. Task의 시작지점을 알게 해 준다.
    std::function<void(CommandList&, TaskContext&)> execute{ nullptr };
};

using LocalTaskID = uint32_t;
static constexpr LocalTaskID InvalidLocalTaskID = std::numeric_limits<LocalTaskID>::max();

struct CompiledTask //RenderGraph에서 pass를 가지고 계산해서 tasks로 만든 결과물.
{
    LocalTaskID localId{ InvalidLocalTaskID };
    Task task{};
    std::vector<uint32_t> dependencies;
    std::vector<uint32_t> dependents;
};