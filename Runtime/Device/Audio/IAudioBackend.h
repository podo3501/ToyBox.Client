#pragma once
#include <memory>

struct IAudioBackend
{
	virtual ~IAudioBackend() = default;
};

std::unique_ptr<IAudioBackend> CreateAudioBackend();