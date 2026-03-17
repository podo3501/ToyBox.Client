#include "pch.h"
#include "StaticSoundInfo.h"

StaticSoundInfo::StaticSoundInfo() :
	SoundInfo{ SoundType::Static }
{}

void StaticSoundInfo::Serialize(Serializer& serializer)
{
	SoundInfo::Serialize(serializer);

	//static 항목이 생기면 여기에 추가
}