#pragma once
#include "GameCore/Service/Asset/AssetData.h"

struct StreamSoundAsset : public AssetData
{
	CORE_DECLARE_TYPE(StreamSoundAsset)

	shared_ptr<IResourceStream> stream;
};