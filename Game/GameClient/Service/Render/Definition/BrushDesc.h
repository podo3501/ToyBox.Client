#pragma once
#include "ResourceDesc.h"
#include "GameClient/Asset/TextureAsset.h"

struct BrushDesc : public ResourceDesc
{
    using ResourceDesc::ResourceDesc;
    bool operator==(const BrushDesc&) const = default;

    virtual Core::TypeID GetAssetTypeID() const override
    {
        return TextureAsset::StaticTypeID();
    }
};