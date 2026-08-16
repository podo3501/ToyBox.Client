#pragma once
#include "MaterialDesc.h"
#include "GameClient/Asset/PbrMaterialAsset.h"

struct PbrMaterialDesc : public MaterialDesc
{
	using MaterialDesc::MaterialDesc; // .material 파일 경로

	virtual Core::TypeID GetAssetTypeID() const override
	{
		return PbrMaterialAsset::StaticTypeID();
	}
};