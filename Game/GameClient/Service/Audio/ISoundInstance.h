#pragma once
#include "PlaybackTypes.h"

struct ISoundInstance
{
	virtual ~ISoundInstance() = default;
	virtual bool Reset(const PlaybackParams& params) = 0;
	virtual bool Play() = 0;
	virtual bool Pause() = 0;
	virtual bool Resume() = 0;
	virtual bool Stop() = 0;
	virtual void Update() = 0;
	virtual bool SetVolume(float volume) = 0;
	virtual PlaybackState GetState() const noexcept = 0;
};
