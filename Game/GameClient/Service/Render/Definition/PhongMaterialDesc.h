#pragma once
#include "MaterialDesc.h"
#include "GameClient/Asset/PhongMaterialAsset.h"

struct PhongMaterialDesc : public MaterialDesc
{
	using MaterialDesc::MaterialDesc; // .material 파일 경로

	virtual Core::TypeID GetAssetTypeID() const override
	{
		return PhongMaterialAsset::StaticTypeID();
	}
};