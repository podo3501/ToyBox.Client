#include "pch.h"
#include "SoundInfo.h"
#include "Audio/SDL/Serializer/Serializer.h"

void SoundInfo::Serialize(Serializer& serializer)
{
	serializer.Process("Filename", filename);
	serializer.Process("SoundLoadMode", loadMode);
	serializer.Process("Group", groupID);
	serializer.Process("Volume", volume);
}
