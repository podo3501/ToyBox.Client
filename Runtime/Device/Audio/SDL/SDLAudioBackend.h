#pragma once
#include "../IAudioBackend.h"

class ISoundBuffer;
class IStaticSoundBuffer;
class StaticSound;
class StreamSound;
class SDLAudioBackend : public IAudioBackend
{
public:
	~SDLAudioBackend();
	SDLAudioBackend();
	virtual bool Initialize(int maxVoices) noexcept override;
	virtual bool InitializE(int maxVoices) noexcept override;
	virtual unique_ptr<IStaticSoundBuffer> CreateStaticSoundBuffer() override;
	//virtual int CreateInstance(ISoundBuffer* sndBuffer, AudioGroupID groupID, float volume) override;
	//virtual ISoundInstance* CreateInstance(ISoundBuffer* sndBuffer, int index, float volume) override;
	virtual ISoundInstance* AcquireInstance(ISoundBuffer* sndBuffer, int index) override;
	virtual bool LoadStream(string_view soundID, unique_ptr<IResourceStream> stream, AudioGroupID groupID, float volume, bool loop) override;
	virtual bool Unload(string_view soundID) noexcept override;
	virtual bool Unload(int handle) noexcept override;
	virtual bool Play(string_view soundID) noexcept override;
	virtual bool Play(int handle) noexcept override;
	virtual void Stop(int instanceHandle) noexcept override;
	virtual bool SetVolume(string_view soundID, float volume) noexcept override;
	virtual bool SetVolume(int handle, float volume) noexcept override;
	virtual void Update() noexcept override;
	virtual PlayState GetState(string_view soundID) const noexcept override;
	virtual PlayState GetState(int handle) const noexcept override;

private:
	unique_ptr<StaticSound> m_staticSound;
	unique_ptr<StreamSound> m_streamSound;
};


