#pragma once
#include "AudioTypes.h"

class Serializer;
struct SoundInfo
{
	void Serialize(Serializer& serializer);

	filesystem::path filename{};
	SoundType sndType{ EnumUtil::Invalid<SoundType> };
	AudioGroupID groupID{ EnumUtil::Invalid<AudioGroupID> };
	float volume{ 0.f };
};