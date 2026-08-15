#pragma once
#include "ResourceDesc.h"
#include "GameClient/Asset/PbrMaterialAsset.h"

struct PbrMaterialDesc : public ResourceDesc
{
	using ResourceDesc::ResourceDesc; // .material 파일 경로
	bool operator==(const PbrMaterialDesc&) const = default;

	virtual Core::TypeID GetAssetTypeID() const override
	{
		return PbrMaterialAsset::StaticTypeID();
	}
};