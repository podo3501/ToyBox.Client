#pragma once
#include <cstdint>
#include <vector>
#include <functional>
#include <variant>
#include "d3dx12.h"
#include "CommandType.h"
#include "TaskHandle.h"
#include "DescriptorAllocation.h"

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
};

struct TaskDesc
{
    CommandType type;
    std::vector<TaskHandle> dependencies; //앞에 Task에 의존하는지. Task의 시작지점을 알게 해 준다.
    std::function<void(CommandList&, TaskContext&)> execute;
    std::function<void(TaskContext&)> onComplete;
};

struct Task
{
    TaskDesc desc;
    TaskContext context;
    std::vector<TaskHandle> dependents; //다른 Task가 나를 의존하고 있는지. 이게 없으면 지울때 뒤에 Task 생각안하고 바로 삭제되버림.

    bool submitted{ false };
    bool finished{ false };
    bool completionCalled{ false };
    uint64_t fenceValue{ 0 };

    std::function<void(TaskContext&)> onComplete;
};