#pragma once
#include <memory>
#include "Core/Foundation/ResourceID.h"

struct AssetData;
using AssetPtr = std::shared_ptr<AssetData>;

using AssetRequestID = uint64_t;
inline constexpr AssetRequestID InvalidAssetRequestID = 0;

struct AssetRequest
{
    Core::ResourceID resID;
    Core::TypeID type{ Core::InvalidTypeID };
};