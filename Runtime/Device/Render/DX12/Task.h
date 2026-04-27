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

struct TextureTransientData
{
    ComPtr<ID3D12Resource> resource;
    DescriptorAllocation srv;
};

struct ResourceContext
{
    std::unordered_map<uint32_t, ComPtr<ID3D12Resource>> textures;
    std::unordered_map<uint32_t, DescriptorAllocation> srvAllocations;

    TextureTransientData TakeTexture(uint32_t id);

    void SetTexture(RGTexture t, ComPtr<ID3D12Resource>&& res) { textures[t.id] = std::move(res); }
    ComPtr<ID3D12Resource>& GetTexture(RGTexture t) { return textures[t.id]; }
    void SetSRV(RGTexture t, DescriptorAllocation&& srv) 
    { 
        srvAllocations[t.id] = std::move(srv); 
    }
    DescriptorAllocation& GetSRV(RGTexture t) { return srvAllocations[t.id]; }
};

inline TextureTransientData ResourceContext::TakeTexture(uint32_t id)
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

struct UploadContext
{
    ComPtr<ID3D12Resource> uploadBuffer;
    //다른 리소스들이 붙을수 있다. 간단하게 구현하기 위해서 variant를 했지만, 나중에는 variant를 제거하는게 목적이다.
};

struct TaskContext
{
    shared_ptr<ResourceContext> resources;
    std::variant<UploadContext> passData;

    void SetTexture(RGTexture t, ComPtr<ID3D12Resource>&& res) { resources->SetTexture(t, std::move(res)); }
    ComPtr<ID3D12Resource>& GetTexture(RGTexture t) { return resources->GetTexture(t); }
    void SetSRV(RGTexture t, DescriptorAllocation&& srv) { resources->SetSRV(t, std::move(srv)); }
    DescriptorAllocation& GetSRV(RGTexture t) { return resources->GetSRV(t); }
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