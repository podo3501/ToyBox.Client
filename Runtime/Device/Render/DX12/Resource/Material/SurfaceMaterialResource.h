#pragma once
#include "MaterialRes.h"
#include "GameClient/Service/Render/Definition/Material/SurfaceMaterialDesc.h"

class SurfaceMaterialResource : public MaterialRes
{
public:
    SurfaceMaterialResource(uint32_t texSlotCount) : MaterialRes{ texSlotCount } {}
    virtual SurfaceType GetSurfaceType() const noexcept = 0;
};