#pragma once
#include "GameCore/Service/Asset/Asset.h"

struct StreamSoundAsset : public Asset
{
	unique_ptr<IResourceStream> stream;
};