#pragma once
#include "VoiceHandle.h"
#include "Soundhandle.h"
#include "Service/Asset/AssetTypes.h"

struct ISoundInstance;
enum class SoundType;
struct Voice
{
	VoiceHandle voiceHandle{};
	SoundHandle soundHandle{};
	ISoundInstance* instance{ nullptr };
	SoundDesc desc{};

	uint32_t playbackTime{ 0 };

	void Reset() noexcept;
	bool StopAndReset() noexcept;
};
