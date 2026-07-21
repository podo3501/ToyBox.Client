#pragma once
#include "MaterialResource.h"
#include "GameClient/Service/Render/Definition/Material/DebugSurfaceMaterialDesc.h"

class DebugSurfaceMaterialResource : public MaterialResource
{
public:
    DebugSurfaceMaterialResource(uint32_t texSlotCount) : MaterialResource{ texSlotCount } {}
    virtual DebugSurfaceType GetDebugSurfaceType() const noexcept = 0;
};