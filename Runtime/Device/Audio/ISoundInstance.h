#pragma once

struct ISoundBuffer;
struct ISoundInstance
{
	virtual ~ISoundInstance() = default;
	virtual bool Reset(float volume) = 0;
	virtual bool Play() = 0;
	virtual bool Pause() = 0;
	virtual bool Stop() = 0;
	virtual bool SetVolume(float volume) = 0;
	virtual bool IsPlaying() const noexcept = 0;
	virtual ISoundBuffer* GetBuffer() noexcept = 0;
};
