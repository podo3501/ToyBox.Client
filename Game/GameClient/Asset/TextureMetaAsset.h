#pragma once
#include "AssetData.h"

struct TextureMetaAsset : public AssetData
{
	CORE_DECLARE_TYPE(TextureMetaAsset)

	bool premultiplyAlpha{ false };
};