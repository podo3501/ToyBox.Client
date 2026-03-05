#pragma once
#include <memory>
#include "AudioTypes.h"

struct IAudioBackend
{
	virtual ~IAudioBackend() = default;
	virtual bool Load(string_view soundID, Core::ByteBuffer buffer, AudioGroupID groupID, float volume) = 0;
	virtual bool Play(string_view soundID) noexcept = 0;
	virtual PlayState GetState(string_view soundID) const noexcept = 0;
	virtual void Update() noexcept = 0;
};

std::unique_ptr<IAudioBackend> CreateAudioBackend();