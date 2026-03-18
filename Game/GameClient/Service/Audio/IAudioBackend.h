#pragma once
#include <memory>

struct IResourceStream;
struct IStaticSoundBuffer;
struct IStreamSoundBuffer;
struct ISoundInstance;
struct ISoundBuffer;
struct IAudioBackend
{
	virtual ~IAudioBackend() = default;
	virtual bool Initialize(int maxVoices, int maxStreams) noexcept = 0;
	virtual unique_ptr<IStaticSoundBuffer> CreateStaticSoundBuffer() = 0;
	virtual unique_ptr<IStreamSoundBuffer> CreateStreamSoundBuffer() = 0;
	virtual ISoundInstance* RequestStaticInstance(ISoundBuffer* sndBuffer, int index) = 0;
	virtual ISoundInstance* RequestStreamInstance(ISoundBuffer* sndBuffer, int index) = 0;
};

std::unique_ptr<IAudioBackend> CreateAudioBackend();