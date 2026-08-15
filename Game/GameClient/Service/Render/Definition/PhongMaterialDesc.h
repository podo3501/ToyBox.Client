#pragma once
#include "ResourceDesc.h"
#include "GameClient/Asset/PhongMaterialAsset.h"

struct PhongMaterialDesc : public ResourceDesc
{
	using ResourceDesc::ResourceDesc; // .material 파일 경로
	bool operator==(const PhongMaterialDesc&) const = default;

	virtual Core::TypeID GetAssetTypeID() const override
	{
		return PhongMaterialAsset::StaticTypeID();
	}
};