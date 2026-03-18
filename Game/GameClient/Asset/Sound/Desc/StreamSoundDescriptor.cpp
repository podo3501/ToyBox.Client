#include "pch.h"
#include "StreamSoundDescriptor.h"
#include "Serializer/Serializer.h"

StreamSoundDescriptor::StreamSoundDescriptor() :
	SoundDescriptor{ SoundType::Stream }
{}

void StreamSoundDescriptor::Serialize(Serializer& serializer)
{
	SoundDescriptor::Serialize(serializer);

	serializer.Process("Loop", loop);
}