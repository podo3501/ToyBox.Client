#pragma once
#include "MaterialDesc.h"

enum class SurfaceType
{
    PBR,
    Grid
};

struct SurfaceMaterialDesc : public MaterialDesc
{
    SurfaceType surfType{ SurfaceType::PBR };

    bool operator==(const SurfaceMaterialDesc&) const = default;
    size_t GetHash() const { return Core::HashOf(surfType, MaterialDesc::GetHash()); }
};