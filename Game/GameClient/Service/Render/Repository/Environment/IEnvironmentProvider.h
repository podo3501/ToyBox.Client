#pragma once
#include "GameClient/Asset/EnvironmentAsset.h"
#include "GameClient/Service/Render/Resource/IEnvironmentResource.h"
#include <memory>

struct IEnvironmentProvider
{
    virtual ~IEnvironmentProvider() = default;
    virtual std::shared_ptr<IEnvironmentResource> CreateResource() = 0;
    virtual bool LoadResource(std::shared_ptr<IEnvironmentResource> res, std::shared_ptr<EnvironmentAsset> asset) = 0;
    virtual void ReleaseResource(std::shared_ptr<IEnvironmentResource> res) = 0;
};
