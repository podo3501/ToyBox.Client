#pragma once
#include "Core/Utils/CycleIterator.h"
#include "InstanceHandle.h"

struct ISoundInstance;
struct SoundDescriptor;
struct LoadedSound;
enum class SoundType;

struct Voice
{
	const LoadedSound* loaded{ nullptr };
	ISoundInstance* instance{ nullptr };
	uint32_t generation{ 0 };

	void Reset() noexcept;
	bool StopAndReset() noexcept;
};

class VoicePool
{
public:
	~VoicePool();
	VoicePool();
	bool Setup(int maxVoices, int maxStreams) noexcept;
	int AcquireVoice(SoundType type, const LoadedSound* loaded, ISoundInstance* instance) noexcept;
	bool StopAllStaticVoices() noexcept;
	bool StopAllStreamVoices() noexcept;
	bool StopStaticVoices(const LoadedSound* loaded) noexcept;
	bool StopStreamVoices(const LoadedSound* loaded) noexcept;
	void UpdateStaticVoices() noexcept;
	void UpdateStreamVoices() noexcept;
	const Voice* GetVoice(int handle) const noexcept;
	Voice* GetVoice(int handle) noexcept;
	const ISoundInstance* GetInstance(int handle) const noexcept;
	ISoundInstance* GetInstance(int handle) noexcept;

private:
	pair<int, Voice*> AcquireVoiceSlot(SoundType type) noexcept;
	int FindFreeVoiceIndex(SoundType type) noexcept;
	bool StopVoices(vector<Voice>& voices, const LoadedSound* loaded) noexcept;
	void UpdateVoices(vector<Voice>& voices) noexcept;

	CycleIterator m_staticCycleIter;
	CycleIterator m_streamCycleIter;
	vector<Voice> m_staticVoices;
	vector<Voice> m_streamVoices;
};
