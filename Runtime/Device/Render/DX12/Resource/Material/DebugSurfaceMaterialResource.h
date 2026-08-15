#pragma once
#include "MaterialRes.h"
#include "GameClient/Service/Render/Definition/Material/DebugSurfaceMaterialDesc.h"

class DebugSurfaceMaterialResource : public MaterialRes
{
public:
    DebugSurfaceMaterialResource(uint32_t texSlotCount) : MaterialRes{ texSlotCount } {}
    virtual DebugSurfaceType GetDebugSurfaceType() const noexcept = 0;
};