#pragma once
#include "Core/Foundation/ResourceID.h"

struct AssetData;

struct IAssetMetaRegistry
{
	virtual ~IAssetMetaRegistry() = default;
	virtual std::shared_ptr<AssetData> GetMeta(const Core::ResourceID& resID) const = 0;
};