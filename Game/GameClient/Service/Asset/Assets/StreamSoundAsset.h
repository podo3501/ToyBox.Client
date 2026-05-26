#pragma once
#include "GameCore/Service/Asset/Asset.h"

struct StreamSoundAsset : public Asset
{
	CORE_DECLARE_TYPE(StreamSoundAsset)

	unique_ptr<IResourceStream> stream;
};