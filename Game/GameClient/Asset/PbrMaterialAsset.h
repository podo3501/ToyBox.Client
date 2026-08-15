#pragma once
#include "MaterialAsset.h"
#include "PbrSurface.h"

struct PbrMaterialAsset : public MaterialAsset
{
    CORE_DECLARE_TYPE(PbrMaterialAsset, MaterialAsset)

    PbrSurface surface;
    std::shared_ptr<TextureAsset> arm{ nullptr };
};