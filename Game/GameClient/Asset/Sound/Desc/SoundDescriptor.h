#pragma once
#include "AudioTypes.h"

class Serializer;
struct SoundDescriptor
{
	virtual ~SoundDescriptor() = default;
	SoundDescriptor() = default;
	explicit SoundDescriptor(SoundType _sndType);
	void Serialize(Serializer& serializer);

	SoundType sndType{ EnumUtil::Invalid<SoundType> };
	filesystem::path filename{};
	AudioGroupID groupID{ EnumUtil::Invalid<AudioGroupID> };
	float volume{ 0.f };
};