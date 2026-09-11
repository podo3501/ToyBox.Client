#pragma once
#include "Core/Foundation/Assert.h"
#include "Graph/RGTypes.h"
#include "Resource/Resource.h"
#include <vector>
#include <optional>

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