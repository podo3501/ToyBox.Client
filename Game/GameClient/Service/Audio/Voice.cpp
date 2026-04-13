#include "pch.h"
#include "Voice.h"
#include "ISoundInstance.h"

void Voice::Reset() noexcept
{
	voiceHandle = {};
	soundHandle = {};
	instance = nullptr;
	desc = {};
	playbackTime = 0;
}

bool Voice::StopAndReset() noexcept
{
	if (!instance) return true;

	instance->Stop();
	Reset();

	return true;
}