#pragma once
#include "GameClient/Service/Asset/Assets/MeshAsset.h"
#include "GameClient/Service/Render/Resource/IMeshResource.h"

struct IMeshSystem
{
    virtual ~IMeshSystem() = default;
    virtual shared_ptr<IMeshResource> CreateMeshResource() = 0;
    virtual bool LoadFromAsset(std::shared_ptr<IMeshResource> resource, std::shared_ptr<MeshAsset> asset) = 0;
};