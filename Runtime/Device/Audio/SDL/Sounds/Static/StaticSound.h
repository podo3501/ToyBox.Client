#pragma once
#include "StaticSoundInst.h"

struct MIX_Mixer;
class ISoundBuffer;
class IStaticSoundBuffer;
class StaticSoundBuffer;
class StaticSoundInstance;
enum class AudioGroupID;
enum class PlayState;

struct InstanceSlot
{
	bool active = false;
	StaticSoundInst inst;
};

class StaticSound
{
public:
	~StaticSound();
	StaticSound();
	bool Initialize();
	unique_ptr<IStaticSoundBuffer> CreateStaticSoundBuffer();
	int CreateInstance(ISoundBuffer* sndBuffer, AudioGroupID groupID, float volume);
	bool Unload(int handle) noexcept;
	bool Play(int handle) noexcept;
	bool Stop(int handle) noexcept;
	PlayState GetState(int handle) const noexcept;
	void SetVolume(AudioGroupID groupID, float volume) noexcept;
	bool SetVolume(int handle, float volume) noexcept;

private:
	const InstanceSlot* GetSlot(int handle) const noexcept;
	InstanceSlot* GetSlot(int handle) noexcept;
	const StaticSoundInst* GetInstance(int handle) const noexcept;
	StaticSoundInst* GetInstance(int handle) noexcept;

	vector<InstanceSlot> m_instances;
	MIX_Mixer* m_mixer{ nullptr };
};
