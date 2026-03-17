#include "pch.h"
#include "StreamSoundInfo.h"
#include "Serializer/Serializer.h"

StreamSoundInfo::StreamSoundInfo() :
	SoundInfo{ SoundType::Stream }
{}

void StreamSoundInfo::Serialize(Serializer& serializer)
{
	SoundInfo::Serialize(serializer);

	serializer.Process("Loop", loop);
}