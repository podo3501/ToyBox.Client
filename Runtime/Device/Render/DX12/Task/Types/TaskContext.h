#pragma once
#include "ResourceContext.h"
#include <memory>

struct TaskContext
{
    std::shared_ptr<ResourceContext> resources; //중요한 리소스. 공유됨.

    void SetResource(RGResourceID id, const Resource& resource) const { resources->Set(id, resource); }
    void SetResource(RGResourceID id, Resource&& resource) const { resources->Set(id, std::move(resource)); }
    Resource& GetResource(RGResourceID id) const { return resources->Get(id); }
};