#pragma once
#include "AssetData.h"
#include "TextureAsset.h"
#include <memory>

enum class MaterialType
{
    Phong,
    PBR
};

struct MaterialAsset : public AssetData
{
    CORE_DECLARE_TYPE(MaterialAsset, AssetData)

    MaterialType type;
    float normalStrength{ 1.f };

    std::shared_ptr<TextureAsset> albedo{ nullptr };
    std::shared_ptr<TextureAsset> normal{ nullptr };
};