#pragma once
#include "AudioTypes.h"

class Serializer;
struct SoundInfo
{
	virtual ~SoundInfo() = default;
	SoundInfo() = default;
	explicit SoundInfo(SoundType _sndType);
	void Serialize(Serializer& serializer);

	SoundType sndType{ EnumUtil::Invalid<SoundType> };
	filesystem::path filename{};
	AudioGroupID groupID{ EnumUtil::Invalid<AudioGroupID> };
	float volume{ 0.f };
};