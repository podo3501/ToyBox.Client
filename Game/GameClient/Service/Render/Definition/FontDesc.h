#pragma once
#include "ResourceDesc.h"
#include "GameClient/Asset/BinaryAsset.h"

struct FontDesc : public ResourceDesc
{
	using ResourceDesc::ResourceDesc; // .ttf 파일 경로

	virtual Core::TypeID GetAssetTypeID() const override
	{
		return BinaryAsset::StaticTypeID();
	}
};