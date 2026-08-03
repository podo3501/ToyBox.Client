#pragma once
#include <memory>
#include "AssetData.h"
#include "Core/Foundation/Types.h"
#include "Core/Foundation/ResourceID.h"

struct IAssetProvider
{
    virtual ~IAssetProvider() = default;
    virtual std::shared_ptr<AssetData> Load(Core::TypeID type, const Core::ResourceID& resID) = 0;
};