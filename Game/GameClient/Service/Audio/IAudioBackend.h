#pragma once
#include <memory>
#include "GameClient/Service/Audio/AudioTypes.h"

struct IResourceStream;
struct ISoundInstance;
struct IStaticSoundBuffer;
struct IStreamSoundBuffer;
struct ISoundBuffer;
struct IAudioBackend
{
	virtual ~IAudioBackend() = default;
	virtual bool Initialize(int maxVoices, int maxStreams) noexcept = 0;
	virtual unique_ptr<IStaticSoundBuffer> CreateStaticSoundBuffer() = 0;
	virtual unique_ptr<IStreamSoundBuffer> CreateStreamSoundBuffer() = 0;
	virtual ISoundInstance* AcquireInstance(SoundType type, ISoundBuffer* sndBuffer, int index) = 0;
};

std::unique_ptr<IAudioBackend> CreateAudioBackend();