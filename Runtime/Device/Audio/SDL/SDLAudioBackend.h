#pragma once
#include "../IAudioBackend.h"

struct MIX_Mixer;
struct ISoundBuffer;
struct IStaticSoundBuffer;
class AudioDevice;
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
	virtual ISoundInstance* AcquireInstance(SoundType type, ISoundBuffer* sndBuffer, int index) override;
	virtual bool LoadStream(string_view soundID, unique_ptr<IResourceStream> stream, AudioGroupID groupID, float volume, bool loop) override;
	virtual void Update() noexcept override;

private:
	bool SetupStaticAudioDevice() noexcept;
	bool SetupStreamAudioDevice() noexcept;
	bool SetupStaticInstances(int maxVoices) noexcept;
	bool SetupStreamInstances(int maxStreams) noexcept;

	MIX_Mixer* m_mixer{ nullptr }; //믹서 안에 Device가 들어가 있다. 
	unique_ptr<AudioDevice> m_streamDevice; //새로운 Device를 만들어서 스트림을 출력시킨다. mixer와 streamDevice의 믹싱은 os에서 해 준다. 서로 다른 장치를 사용하기 때문에 충돌이 나지 않는다.
	vector<StaticSoundInstance> m_staticInstances;
	vector<StreamSoundInstance> m_streamInstances;
};


