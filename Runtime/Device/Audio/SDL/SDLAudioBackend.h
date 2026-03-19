#pragma once
#include "GameClient/Service/Audio/IAudioBackend.h"
#include "AudioMixer.h"
#include "AudioDevice.h"

struct ISoundBuffer;
struct IStaticSoundBuffer;
class StaticSoundInstance;
class StreamSoundInstance;
class SDLAudioBackend : public IAudioBackend
{
public:
	~SDLAudioBackend();
	SDLAudioBackend();
	virtual bool Initialize(int maxVoices, int maxStreams) noexcept override;
	virtual unique_ptr<IStaticSoundBuffer> CreateStaticSoundBuffer() override;
	virtual unique_ptr<IStreamSoundBuffer> CreateStreamSoundBuffer() override;
	virtual ISoundInstance* RequestStaticInstance(ISoundBuffer* sndBuffer) override;
	virtual ISoundInstance* RequestStreamInstance(ISoundBuffer* sndBuffer) override;

private:
	bool SetupStaticInstances(int maxVoices) noexcept;
	bool SetupStreamInstances(int maxStreams) noexcept;

	AudioMixer m_mixer;
	AudioDevice m_streamDevice; //새로운 Device를 만들어서 스트림을 출력시킨다. mixer와 streamDevice의 믹싱은 os에서 해 준다. 서로 다른 장치를 사용하기 때문에 충돌이 나지 않는다.
	vector<StaticSoundInstance> m_staticInstances;
	vector<unique_ptr<StreamSoundInstance>> m_streamInstances;
};


