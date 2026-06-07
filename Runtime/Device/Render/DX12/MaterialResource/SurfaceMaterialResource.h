#pragma once
#include "MaterialResource.h"
#include "GameClient/Service/Render/Desc/SurfaceMaterialDesc.h"

class SurfaceMaterialResource : public MaterialResource
{
public:
    SurfaceMaterialResource(uint32_t texSlotCount) : MaterialResource{ texSlotCount } {}
    virtual SurfaceType GetSurfaceType() const noexcept = 0;
};