#pragma once
#include "MaterialAsset.h"
#include "PhongSurface.h"

struct PhongMaterialAsset : public MaterialAsset
{
    CORE_DECLARE_TYPE(PhongMaterialAsset, MaterialAsset)

    PhongSurface surface;
};