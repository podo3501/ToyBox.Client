#pragma once
#include <memory>
#include "IMaterialResource.h"
#include "ITextureResource.h"

struct IMaterialSystem
{
    virtual ~IMaterialSystem() = default;
    virtual std::shared_ptr<IMaterialResource> CreateMaterialResource(std::shared_ptr<ITextureResource> texRes) = 0;
};