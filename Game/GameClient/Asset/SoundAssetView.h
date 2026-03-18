#pragma once
#include "Sound/StaticSoundDescriptors.h"
#include "Sound/StreamSoundDescriptors.h"

struct SoundAssetView
{
	const StaticSoundDescriptors* staticDescriptors{ nullptr };
	const StreamSoundDescriptors* streamDescriptors{ nullptr };
};