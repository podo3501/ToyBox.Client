#pragma once
#include "ResourceDesc.h"
#include "GameClient/Asset/EnvironmentAsset.h"

struct EnvironmentDesc : public ResourceDesc
{
	using ResourceDesc::ResourceDesc; // .envmap 매니페스트 경로

	virtual Core::TypeID GetAssetTypeID() const override
	{
		return EnvironmentAsset::StaticTypeID();
	}
};