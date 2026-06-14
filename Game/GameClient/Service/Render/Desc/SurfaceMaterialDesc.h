#pragma once
#include "MaterialDesc.h"

enum class SurfaceType
{
    Phong,
    PBR
};

struct SurfaceMaterialDesc : public MaterialDesc
{
    SurfaceType surfType{ SurfaceType::Phong };

    SurfaceMaterialDesc() 
    {
        domain = MaterialDomain::Surface;
    }

    bool operator==(const SurfaceMaterialDesc&) const = default;
    size_t GetHash() const { return Core::HashOf(surfType, MaterialDesc::GetHash()); }
};