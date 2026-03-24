#include "pch.h"
#include "VoicePool.h"
#include "IAudioBackend.h"
#include "ISoundInstance.h"
#include "LoadedSound.h"

static void SortStealCandidateList(vector<Voice*>& stealCandidates)
{
	ranges::sort(stealCandidates, [](Voice* lhs, Voice* rhs) {
		if (lhs->desc.priority != rhs->desc.priority) return lhs->desc.priority < rhs->desc.priority; //낮은 priority 부터 없앤다.
		return lhs->playbackTime > rhs->playbackTime ||
			(lhs->playbackTime == rhs->playbackTime && lhs < rhs); // 오래된 재생 먼저 없앤다.
		});
}

VoicePool::~VoicePool() = default;
VoicePool::VoicePool(IAudioBackend* audioBackend) :
	m_audioBackend{ audioBackend }
{}

bool VoicePool::Setup(int maxVoices, int maxStreams) noexcept
{
	if (maxVoices > MaxHandles) return false;
	if (maxVoices < 0 || maxStreams < 0) return false;
	if (maxVoices < maxStreams) return false;

	m_allocator = HandleAllocator<VoiceTag, 64>(maxVoices);
	m_voices.resize(maxVoices);

	m_maxVoices = maxVoices;
	m_maxStreams = maxStreams;

	return true;
}

VoiceHandle VoicePool::Play(SoundHandle sh, const LoadedSound* loaded, const PlaybackParams& params) noexcept
{
	auto desc = loaded->desc;
	auto vh = AcquireVoiceHandle(desc);
	if (!vh) return vh;

	auto instance = CreateInstance(loaded);
	if (!instance) return InvalidVoiceHandle;

	if (!instance->Reset(params)) return InvalidVoiceHandle;
	if (!instance->Play()) return InvalidVoiceHandle;

	ActivateVoice(vh, sh, instance, desc);
	return vh;
}

ISoundInstance* VoicePool::CreateInstance(const LoadedSound* loaded)
{
	auto type = loaded->desc->sndType;
	auto buffer = loaded->buffer.get();
	switch (type)
	{
	case SoundType::Static: return m_audioBackend->RequestStaticInstance(buffer);
	case SoundType::Stream: return m_audioBackend->RequestStreamInstance(buffer);
	}

	return nullptr;
}

VoiceHandle VoicePool::AcquireVoiceHandle(const SoundDescriptor* desc) noexcept
{
	const bool isStream = (desc->sndType == SoundType::Stream);
	auto& stealList = isStream ? m_stealStreams : m_stealStatics;
	const size_t limit = isStream ? m_maxStreams : m_maxVoices;

	VoiceHandle vh = (stealList.size() >= limit)
		? StealAndAcquire(desc, stealList)
		: m_allocator.Acquire();
	if (!vh) return InvalidVoiceHandle;

	return vh;
}

VoiceHandle VoicePool::StealAndAcquire(const SoundDescriptor* desc, vector<Voice*>& stealList) noexcept
{
	for (auto* voice : stealList)
	{
		if (!voice->instance) continue;
		if (voice->desc.priority >= desc->priority) continue;

		StopVoice(voice->voiceHandle);

		auto vh = m_allocator.Acquire();
		if (vh) return vh;
	}

	return InvalidVoiceHandle;
}

void VoicePool::ActivateVoice(VoiceHandle vh, SoundHandle sh, ISoundInstance* instance, const SoundDescriptor* desc) noexcept
{
	const bool isStream = (desc->sndType == SoundType::Stream);
	auto& stealList = isStream ? m_stealStreams : m_stealStatics;

	uint16_t index = vh.Index();
	Voice& voice = m_voices[index];
	voice.voiceHandle = vh;
	voice.soundHandle = sh;
	voice.instance = instance;
	voice.desc = *desc;
	voice.playbackTime = 0;

	stealList.emplace_back(&voice);
}

bool VoicePool::Pause(VoiceHandle vh) noexcept
{
	auto instance = GetInstance(vh);
	if (instance == nullptr) return false;

	return instance->Pause();
}

bool VoicePool::Resume(VoiceHandle vh) noexcept
{
	auto instance = GetInstance(vh);
	if (instance == nullptr) return false;

	return instance->Resume();
}

static void RemoveFromStealList(vector<Voice*>& voices, Voice* v)
{
	auto it = find(voices.begin(), voices.end(), v);
	if (it != voices.end())
	{
		*it = voices.back();
		voices.pop_back();
	}
}

bool VoicePool::StopVoice(VoiceHandle vh) noexcept
{
	auto voice = GetVoice(vh);
	if (!voice) return false;

	if (voice->desc.sndType == SoundType::Stream) RemoveFromStealList(m_stealStreams, voice);
	if (voice->desc.sndType == SoundType::Static) RemoveFromStealList(m_stealStatics, voice);
	ReturnIfFalse(voice->StopAndReset());

	m_allocator.Release(vh);
	return true;
}

bool VoicePool::StopVoices(SoundHandle sh) noexcept
{
	for (auto& voice : m_voices)
	{
		if (!voice.instance) continue;
		if (voice.soundHandle != sh) continue;

		ReturnIfFalse(StopVoice(voice.voiceHandle));
	}
	return true;
}

bool VoicePool::StopVoices(SoundType type) noexcept
{
	for (auto& voice : m_voices)
	{
		if (!voice.instance) continue;
		if (voice.desc.sndType != type) continue;

		ReturnIfFalse(StopVoice(voice.voiceHandle));
	}
	return true;
}

bool VoicePool::SetVolume(VoiceHandle vh, float volume) noexcept
{
	auto voice = GetVoice(vh);
	if (!voice) return false;

	auto instance = voice->instance;
	if (!instance) return false;

	return instance->SetVolume(volume);
}

PlaybackState VoicePool::GetState(VoiceHandle vh) const noexcept
{
	auto instance = GetInstance(vh);
	if (instance == nullptr) return EnumUtil::Invalid<PlaybackState>;

	return instance->GetState();
}

void VoicePool::UpdateVoices() noexcept
{
	for (auto& voice : m_voices)
	{
		if (!voice.instance) continue;

		voice.instance->Update();
		
		auto state = voice.instance->GetState();
		if (state == PlaybackState::Playing)
			voice.playbackTime += 1; // 재생 중이면 playbackTime값을 프레임 단위로 증가.
		else if (state == PlaybackState::Stopped)
			StopVoice(voice.voiceHandle);
	}

	SortStealCandidateList(m_stealStreams);
	SortStealCandidateList(m_stealStatics);
}

const SoundDescriptor* VoicePool::GetDesc(VoiceHandle vh) const noexcept
{
	auto voice = GetVoice(vh);
	if (!voice) return nullptr;

	return &voice->desc;
}

const Voice* VoicePool::GetVoice(VoiceHandle vh) const noexcept
{
	if (!m_allocator.IsValid(vh))
		return nullptr;

	return &m_voices[vh.Index()];
}

Voice* VoicePool::GetVoice(VoiceHandle vh) noexcept
{
	return const_cast<Voice*>(static_cast<const VoicePool*>(this)->GetVoice(vh));
}

const ISoundInstance* VoicePool::GetInstance(VoiceHandle vh) const noexcept
{
	auto voice = GetVoice(vh);
	return voice ? voice->instance : nullptr;
}

ISoundInstance* VoicePool::GetInstance(VoiceHandle vh) noexcept
{
	return const_cast<ISoundInstance*>(static_cast<const VoicePool*>(this)->GetInstance(vh));
}
