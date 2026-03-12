#pragma once
#include "StaticSoundInst.h"
#include "Core/Utils/CycleIterator.h"

struct MIX_Mixer;
struct ISoundInstance;
class ISoundBuffer;
class IStaticSoundBuffer;
class StaticSoundBuffer;
class StaticSoundInstance;
enum class AudioGroupID;
enum class PlayState;

struct VoicE
{
	bool active{ false };
	uint32_t generation{ 0 };
	StaticSoundInst inst;
};

class StaticSound
{
public:
	~StaticSound();
	StaticSound();
	bool Initialize(int maxVoices);
	bool InitializE(int maxVoices);
	unique_ptr<IStaticSoundBuffer> CreateStaticSoundBuffer();
	//int CreateInstance(ISoundBuffer* sndBuffer, AudioGroupID groupID, float volume);
	ISoundInstance* AcquireInstance(StaticSoundBuffer* buffer, int index) noexcept;
	bool Unload(int handle) noexcept;
	bool Play(int handle) noexcept;
	bool Stop(int handle) noexcept;
	PlayState GetState(int handle) const noexcept;
	void SetVolume(AudioGroupID groupID, float volume) noexcept;
	bool SetVolume(int handle, float volume) noexcept;

private:
	const VoicE* GetVoice(int handle) const noexcept;
	VoicE* GetVoice(int handle) noexcept;
	const StaticSoundInst* GetInstance(int handle) const noexcept;
	StaticSoundInst* GetInstance(int handle) noexcept;

	vector<VoicE> m_voices;
	MIX_Mixer* m_mixer{ nullptr };
	CycleIterator m_cycleIter;

	vector<StaticSoundInst> m_instances;
};
