#pragma once
#include "../IAudioBackend.h"
#include "Static/StaticSoundInstance.h"

struct ISoundBuffer;
struct IStaticSoundBuffer;
class StaticSound;
class StreamSound;
class SDLAudioBackend : public IAudioBackend
{
public:
	~SDLAudioBackend();
	SDLAudioBackend();
	virtual bool Initialize(int maxVoices) noexcept override;
	virtual unique_ptr<IStaticSoundBuffer> CreateStaticSoundBuffer() override;
	virtual unique_ptr<IStreamSoundBuffer> CreateStreamSoundBuffer() override;
	virtual ISoundInstance* AcquireInstance(ISoundBuffer* sndBuffer, int index) override;
	virtual bool LoadStream(string_view soundID, unique_ptr<IResourceStream> stream, AudioGroupID groupID, float volume, bool loop) override;
	virtual void Update() noexcept override;

private:
	bool SetupAudioDevice() noexcept;
	bool SetupStaticInstances(int maxVoices) noexcept;

	MIX_Mixer* m_mixer{ nullptr };
	vector<StaticSoundInstance> m_instances;
	unique_ptr<StreamSound> m_streamSound;
};


