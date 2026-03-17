#pragma once
#include "SoundInfo.h"

class Serializer;
struct StreamSoundInfo : public SoundInfo
{
	StreamSoundInfo();
	void Serialize(Serializer& serializer);

	bool loop{ false };
};
