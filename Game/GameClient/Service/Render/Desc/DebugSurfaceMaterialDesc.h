#pragma once
#include "MaterialDesc.h"

enum class DebugSurfaceType
{
    Grid
};

struct DebugSurfaceMaterialDesc : public MaterialDesc
{
    DebugSurfaceType debugSurfType{ DebugSurfaceType::Grid };

    bool operator==(const DebugSurfaceMaterialDesc&) const = default;
    size_t GetHash() const { return Core::HashOf(debugSurfType, MaterialDesc::GetHash()); }
};