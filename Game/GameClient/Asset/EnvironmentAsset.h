#pragma once
#include "AssetData.h"
#include "TextureCubeAsset.h"
#include "SphericalHarmonicsAsset.h"

struct EnvironmentAsset : public AssetData
{
    CORE_DECLARE_TYPE(EnvironmentAsset)

    std::shared_ptr<TextureCubeAsset> skybox;
    std::shared_ptr<TextureCubeAsset> reflection;
    std::shared_ptr<SphericalHarmonicsAsset> irradiance;
};