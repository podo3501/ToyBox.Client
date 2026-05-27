#pragma once
#include <memory>

struct IResourceStream;
struct ISoundInstance;
struct ISoundBuffer;
struct IAudioBackend
{
	virtual ~IAudioBackend() = default;
	virtual bool Initialize(int maxVoices, int maxStreams) noexcept = 0;
	virtual unique_ptr<ISoundBuffer> CreateStaticSoundBuffer() = 0;
	virtual unique_ptr<ISoundBuffer> CreateStreamSoundBuffer() = 0;
	virtual ISoundInstance* RequestStaticInstance(ISoundBuffer* sndBuffer) = 0;
	virtual ISoundInstance* RequestStreamInstance(ISoundBuffer* sndBuffer) = 0;
};

std::unique_ptr<IAudioBackend> CreateAudioBackend();