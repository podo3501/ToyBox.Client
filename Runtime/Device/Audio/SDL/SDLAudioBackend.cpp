#include "pch.h"
#include "SDLAudioBackend.h"

unique_ptr<IAudioBackend> CreateAudioBackend()
{
	return make_unique<SDLAudioBackend>();
}