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
	AudioGroup group{ EnumUtil::Invalid<AudioGroup> };
	int priority{ 0 };
	float volume{ 0.f };
};