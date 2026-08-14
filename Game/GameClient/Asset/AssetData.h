#pragma once
#include "Core/Foundation/Types.h"
#include <vector>

struct AssetData
{
public:
    virtual ~AssetData() = default;

    static Core::TypeID StaticTypeID() { return Core::GetTypeID<AssetData>(); }
    virtual Core::TypeID GetTypeID() const { return StaticTypeID(); }
};