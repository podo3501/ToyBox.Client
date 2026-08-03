#pragma once
#include "AssetData.h"
#include "Core/Math/Vector3.h"

struct SphericalHarmonicsAsset : public AssetData
{
    CORE_DECLARE_TYPE(SphericalHarmonicsAsset)

    std::array<Core::Vector3, 9> coefficients;
};