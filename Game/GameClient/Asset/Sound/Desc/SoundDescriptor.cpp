#include "pch.h"
#include "SoundDescriptor.h"
#include "Serializer/Serializer.h"

SoundDescriptor::SoundDescriptor(SoundType _sndType) :
	sndType{ _sndType }
{}

void SoundDescriptor::Serialize(Serializer& serializer)
{
	serializer.Process("Filename", filename);
	serializer.Process("Group", groupID);
	serializer.Process("Volume", volume);
}
