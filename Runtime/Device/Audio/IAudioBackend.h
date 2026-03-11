#pragma once
#include <memory>
#include "AudioTypes.h"

struct IResourceStream;
class ISoundBuffer;
class IStaticSoundBuffer;
struct IAudioBackend
{
	virtual ~IAudioBackend() = default;
	virtual unique_ptr<IStaticSoundBuffer> CreateStaticSoundBuffer() = 0;
	virtual int CreateInstance(ISoundBuffer* sndBuffer, AudioGroupID groupID, float volume) = 0;
	//virtual int LoadStatic(string_view soundID, Core::ByteBuffer buffer, AudioGroupID groupID, float volume) = 0;
	virtual bool LoadStream(string_view soundID, unique_ptr<IResourceStream> stream, AudioGroupID groupID, float volume, bool loop) = 0;
	virtual bool Unload(string_view soundID) noexcept = 0;
	virtual bool Unload(int handle) noexcept = 0;
	virtual bool Play(string_view soundID) noexcept = 0;
	virtual bool Play(int handle) noexcept = 0;
	virtual bool SetVolume(string_view soundID, float volume) noexcept = 0;
	virtual bool SetVolume(int handle, float volume) noexcept = 0;
	virtual PlayState GetState(string_view soundID) const noexcept = 0;
	virtual PlayState GetState(int handle) const noexcept = 0;
	virtual void Update() noexcept = 0;
};

std::unique_ptr<IAudioBackend> CreateAudioBackend();