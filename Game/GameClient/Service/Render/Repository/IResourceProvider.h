#pragma once
#include "GameClient/Asset/AssetData.h"
#include "GameClient/Service/Render/Resource/IResource.h"
#include <memory>

struct IResourceProvider
{
    virtual ~IResourceProvider() = default;
    virtual std::shared_ptr<IResource> CreateResource(std::shared_ptr<AssetData> asset) = 0;
    virtual void ReleaseResource(std::shared_ptr<IResource> res) = 0;
};