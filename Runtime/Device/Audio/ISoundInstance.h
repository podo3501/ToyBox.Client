#pragma once

class ISoundBuffer;
struct ISoundInstance
{
	virtual ~ISoundInstance() = default;
	virtual bool Play() = 0;
	virtual bool Reset(float volume) = 0;
	virtual bool IsPlaying() const noexcept = 0;
};
