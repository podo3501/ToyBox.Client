#include "pch.h"
#include "VoicePool.h"
#include "ISoundInstance.h"
#include "Asset/Sound/Desc/AudioTypes.h"

void Voice::Reset() noexcept
{
	loaded = nullptr;
	instance = nullptr;
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
VoicePool::VoicePool() = default;
bool VoicePool::Setup(int maxVoices, int maxStreams) noexcept
{
	if (maxVoices < 0 || maxStreams < 0) return false;

	if (maxVoices > 0) m_staticCycleIter.SetRange(0, maxVoices); //인덱스를 차례로 사용하게 하는 순환인덱스 생성.
	if (maxStreams > 0) m_streamCycleIter.SetRange(0, maxStreams);

	m_staticVoices.resize(maxVoices);
	m_streamVoices.resize(maxStreams);

	return true;
}

int VoicePool::AcquireVoice(SoundType type, const LoadedSound* loaded, ISoundInstance* instance) noexcept
{
	auto [index, voice] = AcquireVoiceSlot(type);
	if (voice == nullptr) return 0;

	voice->loaded = loaded;
	voice->instance = instance;

	return index;
}

static int MakeHandle(uint16_t index, uint16_t generation)
{
	return (index & 0xFFFF) | (generation << 16); // 32비트 핸들 생성: 상위 16비트 = 세대, 하위 16비트 = 슬롯 인덱스;
}

static pair<uint16_t, uint16_t> SplitHandle(int handle)
{
	uint16_t index = static_cast<uint16_t>(handle & 0xFFFF);
	uint16_t generation = static_cast<uint16_t>((handle >> 16) & 0xFFFF);
	return { index, generation };
}

pair<int, Voice*> VoicePool::AcquireVoiceSlot(SoundType type) noexcept
{
	int index = FindFreeVoiceIndex(type);
	if (index == -1) return { -1, nullptr };

	Voice* voice = nullptr;
	int baseIndex = 0;

	if (type == SoundType::Static)
	{
		voice = &m_staticVoices[index];
		baseIndex = 0;
	}
	else // SoundType::Stream
	{
		voice = &m_streamVoices[index];
		baseIndex = static_cast<int>(m_staticVoices.size());
	}
	if (voice == nullptr) return { -1, nullptr };

	voice->generation = (voice->generation + 1) & 0xFFFF;
	int handleIndex = MakeHandle(baseIndex + index, voice->generation);
	return { handleIndex, voice };
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

bool VoicePool::StopAllStaticVoices() noexcept
{
	return ranges::all_of(m_staticVoices, [](auto& voice) {
		return voice.StopAndReset();
		});
}

bool VoicePool::StopAllStreamVoices() noexcept
{
	return ranges::all_of(m_streamVoices, [](auto& voice) {
		return voice.StopAndReset();
		});
}

bool VoicePool::StopStaticVoices(const LoadedSound* loaded) noexcept { return StopVoices(m_staticVoices, loaded); }
bool VoicePool::StopStreamVoices(const LoadedSound* loaded) noexcept { return StopVoices(m_streamVoices, loaded); }
bool VoicePool::StopVoices(vector<Voice>& voices, const LoadedSound* loaded) noexcept
{
	for (auto& voice : voices)
	{
		if (voice.loaded != loaded) continue;
		ReturnIfFalse(voice.StopAndReset());
	}
	return true;
}

void VoicePool::UpdateStaticVoices() noexcept { UpdateVoices(m_staticVoices); }
void VoicePool::UpdateStreamVoices() noexcept { UpdateVoices(m_streamVoices); }
void VoicePool::UpdateVoices(vector<Voice>& voices) noexcept
{
	for (auto& voice : voices)
	{
		if (!voice.instance) continue;

		voice.instance->Update();
		if (voice.instance->GetState() == PlaybackState::Stopped)
			voice.Reset();
	}
}

const Voice* VoicePool::GetVoice(int handle) const noexcept
{
	if (handle <= 0) return nullptr;

	auto [index, gen] = SplitHandle(handle);
	if (index >= m_staticVoices.size() + m_streamVoices.size()) return nullptr;

	const Voice* voice = (index < m_staticVoices.size())
		? &m_staticVoices[index]
		: &m_streamVoices[index - m_staticVoices.size()];

	if (voice->instance == nullptr || voice->generation != gen) // instance가 없거나 세대가 다르면 nullptr 반환
		return nullptr;

	return voice;
}

Voice* VoicePool::GetVoice(int handle) noexcept
{
	return const_cast<Voice*>(static_cast<const VoicePool*>(this)->GetVoice(handle));
}

const ISoundInstance* VoicePool::GetInstance(int handle) const noexcept
{
	auto voice = GetVoice(handle);
	if (voice == nullptr) return nullptr;

	return voice->instance;
}

ISoundInstance* VoicePool::GetInstance(int handle) noexcept
{
	return const_cast<ISoundInstance*>(static_cast<const VoicePool*>(this)->GetInstance(handle));
}