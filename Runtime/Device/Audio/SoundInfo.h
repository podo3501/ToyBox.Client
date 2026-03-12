#pragma once
#include "AudioTypes.h"

class Serializer;
struct SoundInfo
{
	void Serialize(Serializer& serializer);

	filesystem::path filename{};
	SoundType sndType{ SoundType::None };
	AudioGroupID groupID{ AudioGroupID::None };
	float volume{ 0.f };
};