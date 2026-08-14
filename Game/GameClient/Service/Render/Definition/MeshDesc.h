#pragma once
#include "ResourceDesc.h"
#include "GameClient/Asset/MeshAsset.h"

struct MeshDesc : public ResourceDesc
{
	using ResourceDesc::ResourceDesc;
	bool operator==(const MeshDesc&) const = default;

	virtual Core::TypeID GetAssetTypeID() const override
	{
		return MeshAsset::StaticTypeID();
	}
};