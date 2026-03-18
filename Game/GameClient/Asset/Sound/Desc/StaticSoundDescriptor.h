#pragma once
#include "SoundDescriptor.h"

class Serializer;
struct StaticSoundDescriptor : public SoundDescriptor
{
	StaticSoundDescriptor();
	void Serialize(Serializer& serializer);

	//static 항목이 생기면 여기에 추가
};
