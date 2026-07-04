#pragma once
#include "GameClient/Asset/MeshAsset.h"
#include "GameClient/Service/Render/Resource/IMeshResource.h"

struct IMeshProvider
{
    virtual ~IMeshProvider() = default;
    virtual shared_ptr<IMeshResource> CreateResource() = 0;
    virtual bool LoadResource(std::shared_ptr<IMeshResource> resource, std::shared_ptr<MeshAsset> asset) = 0;
    virtual void ReleaseResource(std::shared_ptr<IMeshResource> resource) = 0;
};