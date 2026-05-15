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
#include <utility>
#include <memory>
#include "GameClient/Graphics/RenderData/CameraData.h"

class CommandList;

using Microsoft::WRL::ComPtr;

struct IInternalData 
{ 
    virtual ~IInternalData() = default; 
    virtual const std::type_info& Type() const = 0;
};

template<typename T>
struct DataWrapper : public IInternalData //다양한 타입을 넣을수 있도록(variant나 void*를 쓰면 코드 짜기가 불편하다)
{
    T data;
    template<typename U>
    DataWrapper(U&& val) : data(std::forward<U>(val)) {}

    const std::type_info& Type() const override { return typeid(T); }
};

struct ResourceContext
{
    std::unordered_map<uint32_t, std::unique_ptr<IInternalData>> resources;

    template<typename T>
    void Set(RGHandle h, T&& res)
    {
        resources[h.id] = std::make_unique<DataWrapper<T>>(std::forward<T>(res));
    }

    template<typename T>
    T& Get(RGHandle h)
    {
        auto* wrapper = static_cast<DataWrapper<T>*>(resources[h.id].get());
        return wrapper->data;
    }
};

struct UploadContext
{
    ComPtr<ID3D12Resource> resource;
};

struct PassContext {
    std::unique_ptr<IInternalData> data;

    template<typename T>
    void Set(T&& val) 
    {
        data = std::make_unique<DataWrapper<T>>(std::forward<T>(val));
    }

    template<typename T>
    T& Get() 
    {
        Assert(data);
        Assert(data->Type() == typeid(T));

        auto* wrapper = static_cast<DataWrapper<T>*>(data.get());
        return wrapper->data;
    }
};

struct TaskContext
{
    shared_ptr<ResourceContext> resources; //중요한 리소스. 공유됨.
    shared_ptr<UploadContext> upload; //중요한 리소스. 공유됨.
    PassContext passContext; //pass가 실행되면서 임시로 생겼다가 사라지는 것들. 공유 안됨.
    CameraData camera;

    template<typename T>
    void SetResource(RGHandle h, T&& res) { resources->Set<T>(h, std::forward<T>(res)); }
    template<typename T>
    T& GetResource(RGHandle h) { return resources->Get<T>(h); }

    template<typename T>
    void SetPassContext(T&& data) { passContext.Set<T>(std::forward<T>(data)); }
    template<typename T>
    T& GetPassContext() { return passContext.Get<T>(); }
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