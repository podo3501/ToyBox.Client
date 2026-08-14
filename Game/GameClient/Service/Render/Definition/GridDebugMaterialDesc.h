#pragma once
#include "DebugMaterialDesc.h"

struct GridDebugMaterialDesc : public DebugMaterialDesc
{
    explicit GridDebugMaterialDesc(Core::ResourceID resID) :
        DebugMaterialDesc{ resID, DebugMaterialType::Grid }
    {}
};