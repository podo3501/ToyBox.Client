#pragma once
#include "GameCore/Service/Asset/Asset.h"

struct StreamSoundAsset : public Asset
{
	CORE_DECLARE_TYPE(StreamSoundAsset)

	shared_ptr<IResourceStream> stream;
};