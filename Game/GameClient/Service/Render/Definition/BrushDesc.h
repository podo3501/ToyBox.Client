#pragma once
#include "ResourceDesc.h"
#include "GameClient/Asset/TextureAsset.h"

struct BrushDesc : public ResourceDesc
{
    using ResourceDesc::ResourceDesc;

    virtual Core::TypeID GetAssetTypeID() const override
    {
        return TextureAsset::StaticTypeID();
    }
};