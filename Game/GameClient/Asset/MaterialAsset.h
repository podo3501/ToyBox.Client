#pragma once
#include "AssetData.h"
#include "MaterialType.h"
#include "TextureAsset.h"
#include <memory>

struct MaterialAsset : public AssetData
{
    CORE_DECLARE_TYPE(MaterialAsset, AssetData)

    MaterialType type;

    std::shared_ptr<TextureAsset> albedo{ nullptr };
    std::shared_ptr<TextureAsset> normal{ nullptr };
};