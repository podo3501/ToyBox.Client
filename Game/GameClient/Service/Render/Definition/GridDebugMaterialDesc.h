#pragma once
#include "Core/Utils/Hash.h"

struct GridDebugMaterialDesc
{
    Core::ResourceID resID;

    bool operator==(const GridDebugMaterialDesc&) const = default;
    size_t GetHash() const { return Core::HashOf(resID); }
};