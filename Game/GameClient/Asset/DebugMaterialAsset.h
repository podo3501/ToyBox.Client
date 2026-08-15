#pragma once
#include "AssetData.h"
#include "DebugMaterialType.h"

struct DebugMaterialAsset : public AssetData
{
    CORE_DECLARE_TYPE(DebugMaterialAsset, AssetData)

    DebugMaterialType type;
};