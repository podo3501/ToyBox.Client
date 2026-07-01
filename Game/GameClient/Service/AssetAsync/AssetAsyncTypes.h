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

template<typename T>
AssetRequest MakeAssetRequest(Core::ResourceID resID)
{
    return
    {
        .resID = std::move(resID),
        .type = Core::GetTypeID<T>()
    };
}

template<typename T>
AssetRequest MakeAssetRequest(std::string_view path)
{
    return MakeAssetRequest<T>(Core::ResourceID::MakePath(path));
}