#include "pch.h"
#include "StaticSoundDescriptor.h"

StaticSoundDescriptor::StaticSoundDescriptor() :
	SoundDescriptor{ SoundType::Static }
{}

void StaticSoundDescriptor::Serialize(Serializer& serializer)
{
	SoundDescriptor::Serialize(serializer);

	//static 항목이 생기면 여기에 추가
}