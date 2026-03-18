#pragma once
#include "SoundDescriptor.h"

class Serializer;
struct StreamSoundDescriptor : public SoundDescriptor
{
	StreamSoundDescriptor();
	void Serialize(Serializer& serializer);

	bool loop{ false };
};
