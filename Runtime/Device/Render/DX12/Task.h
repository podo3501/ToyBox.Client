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

struct UploadContext
{
    ComPtr<ID3D12Resource> uploadBuffer;
};

struct TextureTransientData
{
    ComPtr<ID3D12Resource> resource;
    DescriptorAllocation srv;
};

struct FrameResources
{
    std::unordered_map<uint32_t, ComPtr<ID3D12Resource>> textures;
    std::unordered_map<uint32_t, DescriptorAllocation> srvAllocations;

    TextureTransientData TakeTexture(uint32_t id);

    void SetTexture(RGTexture t, ComPtr<ID3D12Resource>&& res) { textures[t.id] = std::move(res); }
    ComPtr<ID3D12Resource>& GetTexture(RGTexture t) { return textures[t.id]; }
    void SetSRV(RGTexture t, DescriptorAllocation&& srv) { srvAllocations[t.id] = std::move(srv); }
    DescriptorAllocation& GetSRV(RGTexture t) { return srvAllocations[t.id]; }
};

inline TextureTransientData FrameResources::TakeTexture(uint32_t id)
{
    TextureTransientData data;

    auto itRes = textures.find(id);
    auto itSrv = srvAllocations.find(id);

    assert(itRes != textures.end());
    assert(itSrv != srvAllocations.end());

    data.resource = std::move(itRes->second);
    data.srv = std::move(itSrv->second);

    textures.erase(itRes);
    srvAllocations.erase(itSrv);

    return data;
}

struct TaskContext
{
    shared_ptr<FrameResources> resources;
    std::variant<UploadContext> passData;

    void SetTexture(RGTexture t, ComPtr<ID3D12Resource>&& res) { resources->SetTexture(t, std::move(res)); }
    ComPtr<ID3D12Resource>& GetTexture(RGTexture t) { return resources->GetTexture(t); }
    void SetSRV(RGTexture t, DescriptorAllocation&& srv) { resources->SetSRV(t, std::move(srv)); }
    DescriptorAllocation& GetSRV(RGTexture t) { return resources->GetSRV(t); }
};

struct TaskDesc
{
    CommandType type;
    std::vector<TaskHandle> dependencies; //앞에 Task에 의존하는지. Task의 시작지점을 알게 해 준다.
    std::function<void(CommandList&, TaskContext&)> gpuExecute = [](CommandList&, const TaskContext&) {};
    std::function<void(TaskContext&)> cpuExecute = [](const TaskContext&) {};
};

struct Task
{
    TaskDesc desc;
    TaskContext context;
    std::vector<TaskHandle> dependents; //다른 Task가 나를 의존하고 있는지. 이게 없으면 지울때 뒤에 Task 생각안하고 바로 삭제되버림.

    bool submitted{ false };
    bool finished{ false };
    uint64_t fenceValue{ 0 };
};