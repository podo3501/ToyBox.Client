#include "pch.h"
#include "SoundInfo.h"
#include "Serializer/Serializer.h"

SoundInfo::SoundInfo(SoundType _sndType) :
	sndType{ _sndType }
{}

void SoundInfo::Serialize(Serializer& serializer)
{
	serializer.Process("Filename", filename);
	serializer.Process("Group", groupID);
	serializer.Process("Volume", volume);
}
