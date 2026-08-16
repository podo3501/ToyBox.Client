#pragma once
#include "ResourceDesc.h"
#include "GameClient/Asset/MeshAsset.h"

struct DebugMeshDesc : public ResourceDesc
{
    using ResourceDesc::ResourceDesc;

    virtual Core::TypeID GetAssetTypeID() const override
    {
        return MeshAsset::StaticTypeID();
    }
};