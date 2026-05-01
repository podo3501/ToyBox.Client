#pragma once
#include <cstdint>
#include <vector>
#include <functional>
#include <variant>
#include "d3dx12.h"
#include "CommandType.h"
#include "TaskHandle.h"
#include "DescriptorAllocation.h"
#include "RGTypes.h"

class CommandList;

using Microsoft::WRL::ComPtr;

struct ResourceContext
{
    std::unordered_map<uint32_t, ComPtr<ID3D12Resource>> resources;

    void SetResource(RGResource h, ComPtr<ID3D12Resource>&& res) { resources[h.id] = std::move(res); }
    ComPtr<ID3D12Resource>& GetResource(RGResource h) { return resources[h.id]; }
};

struct UploadContext
{
    ComPtr<ID3D12Resource> uploadBuffer;
    //다른 리소스들이 붙을수 있다. 간단하게 구현하기 위해서 variant를 했지만, 나중에는 variant를 제거하는게 목적이다.
};

struct TaskContext
{
    shared_ptr<ResourceContext> resources;
    std::variant<UploadContext> passData;

    void SetResource(RGResource h, ComPtr<ID3D12Resource>&& res) { resources->SetResource(h, std::move(res)); }
    ComPtr<ID3D12Resource>& GetResource(RGResource h) { return resources->GetResource(h); }
};

struct Task
{
    CommandType type;
    std::vector<TaskHandle> dependencies; //앞에 Task에 의존하는지. Task의 시작지점을 알게 해 준다.
    std::function<void(CommandList&, TaskContext&)> gpuExecute = [](CommandList&, const TaskContext&) {};
    std::function<void(TaskContext&)> cpuExecute = [](const TaskContext&) {};
};

struct CompiledTask //RenderGraph에서 pass를 가지고 계산해서 tasks로 만든 결과물.
{
    TaskHandle handle;
    Task task;
    std::vector<TaskHandle> dependents;
};