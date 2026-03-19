#pragma once
#include "Core/Utils/CycleIterator.h"

struct ISoundInstance;
struct SoundDescriptor;
enum class SoundType;

struct Voice
{
	ISoundInstance* instance{ nullptr };
	const SoundDescriptor* desc{ nullptr };
	uint32_t generation{ 0 };

	void Reset() noexcept;
	bool StopAndReset() noexcept;
};

class VoicePool
{
public:
	~VoicePool();
	VoicePool(int maxVoices, int maxStreams) noexcept;
	int FindFreeVoiceIndex(SoundType type) noexcept;

private:
	CycleIterator m_staticCycleIter;
	CycleIterator m_streamCycleIter;
	vector<Voice> m_staticVoices;
	vector<Voice> m_streamVoices;
};
