#pragma once
#include "AssetData.h"

enum class DebugMaterialType
{
    Grid
};

struct DebugMaterialAsset : public AssetData
{
    CORE_DECLARE_TYPE(DebugMaterialAsset)

    DebugMaterialType type;
};