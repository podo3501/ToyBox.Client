#pragma once
#include "SoundInfo.h"

class Serializer;
struct StaticSoundInfo : public SoundInfo
{
	StaticSoundInfo();
	void Serialize(Serializer& serializer);

	//static 항목이 생기면 여기에 추가
};
