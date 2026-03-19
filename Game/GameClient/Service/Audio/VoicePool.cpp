#include "pch.h"
#include "VoicePool.h"
#include "ISoundInstance.h"
#include "Asset/Sound/Desc/AudioTypes.h"

void Voice::Reset() noexcept
{
	instance = nullptr;
	desc = nullptr;
}

bool Voice::StopAndReset() noexcept
{
	if (!instance) return true;

	ReturnIfFalse(instance->Stop());
	Reset();

	return true;
}

////////////////////////////////////////////////

VoicePool::~VoicePool() = default;
VoicePool::VoicePool(int maxVoices, int maxStreams) noexcept
{
	if (maxVoices > 0) m_staticCycleIter.SetRange(0, maxVoices); //인덱스를 차례로 사용하게 하는 순환인덱스 생성.
	if (maxStreams > 0) m_streamCycleIter.SetRange(0, maxStreams);

	m_staticVoices.resize(maxVoices);
	m_streamVoices.resize(maxStreams);
}

int VoicePool::FindFreeVoiceIndex(SoundType type) noexcept
{
	auto& voices = (type == SoundType::Static) ? m_staticVoices : m_streamVoices;
	auto& iter = (type == SoundType::Static) ? m_staticCycleIter : m_streamCycleIter;
	if (voices.empty()) return -1;

	for (size_t i = 0; i < voices.size(); ++i)
	{
		int index = iter.Increase();
		auto& voice = voices[index];
		if (voice.instance != nullptr) continue;

		return index;
	}

	return -1;
}