#pragma once
#include "../AudioTypes.h"

class Serializer;
struct SoundInfo
{
	void Serialize(Serializer& serializer);

	string filename{};
	AudioGroupID groupID{ AudioGroupID::None };
	float volume{ 0.f };
};