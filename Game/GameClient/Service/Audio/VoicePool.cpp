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

	m_voices.Setup(maxVoices);

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
	if (!instance) return VoiceHandle::Invalid();

	if (!instance->Reset(params)) return VoiceHandle::Invalid();
	if (!instance->Play()) return VoiceHandle::Invalid();

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

VoiceHandle VoicePool::AcquireVoiceHandle(const SoundDesc* desc) noexcept
{
	const bool isStream = (desc->sndType == SoundType::Stream);
	auto& stealList = isStream ? m_stealStreams : m_stealStatics;
	const size_t limit = isStream ? m_maxStreams : m_maxVoices;

	VoiceHandle vh = (stealList.size() >= limit)
		? StealAndAcquire(desc, stealList)
		: m_voices.Emplace();
	if (!vh) return VoiceHandle::Invalid();

	return vh;
}

VoiceHandle VoicePool::StealAndAcquire(const SoundDesc* desc, vector<Voice*>& stealList) noexcept
{
	for (auto* voice : stealList)
	{
		if (!voice->instance) continue;
		if (voice->desc.priority >= desc->priority) continue;

		StopVoice(voice->voiceHandle);

		auto vh = m_voices.Emplace();
		if (vh) return vh;
	}

	return VoiceHandle::Invalid();
}

void VoicePool::ActivateVoice(VoiceHandle vh, SoundHandle sh, ISoundInstance* instance, const SoundDesc* desc) noexcept
{
	auto* voice = m_voices.Get(vh);
	if (!voice) return;

	const bool isStream = (desc->sndType == SoundType::Stream);
	auto& stealList = isStream ? m_stealStreams : m_stealStatics;

	voice->voiceHandle = vh;
	voice->soundHandle = sh;
	voice->instance = instance;
	voice->desc = *desc;
	voice->playbackTime = 0;

	stealList.emplace_back(voice);
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
	auto voice = m_voices.Get(vh);
	if (!voice) return false;

	if (voice->desc.sndType == SoundType::Stream) RemoveFromStealList(m_stealStreams, voice);
	if (voice->desc.sndType == SoundType::Static) RemoveFromStealList(m_stealStatics, voice);
	ReturnIfFalse(voice->StopAndReset());

	m_voices.Remove(vh);
	return true;
}

bool VoicePool::StopVoices(SoundHandle sh) noexcept
{
	std::vector<VoiceHandle> toStop;

	m_voices.Visit([&toStop, sh](VoiceHandle h, Voice& voice) {
		if (!voice.instance) return;
		if (voice.soundHandle == sh)
			toStop.push_back(h);
		});

	for (auto& h : toStop)
		ReturnIfFalse(StopVoice(h));

	return true;
}

bool VoicePool::StopVoices(SoundType type) noexcept
{
	std::vector<VoiceHandle> toStop;

	m_voices.Visit([&toStop, type](VoiceHandle h, Voice& voice) {
		if (!voice.instance) return;
		if (voice.desc.sndType == type)
			toStop.push_back(h);
		});

	for (auto& h : toStop)
		ReturnIfFalse(StopVoice(h));

	return true;
}

bool VoicePool::SetVolume(VoiceHandle vh, float volume) noexcept
{
	auto voice = m_voices.Get(vh);
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
	std::vector<VoiceHandle> toStop; //삭제할 것을 넣어놓는 이유는 visit안에서 삭제하게 되면 iterator가 꼬여서 깨지기 때문이다.

	m_voices.Visit([&](VoiceHandle h, Voice& voice) {
		if (!voice.instance) return;

		voice.instance->Update();

		auto state = voice.instance->GetState();
		if (state == PlaybackState::Playing)
			voice.playbackTime += 1;
		else if (state == PlaybackState::Stopped)
			toStop.push_back(h);
		});

	for (auto& h : toStop)
		StopVoice(h);

	SortStealCandidateList(m_stealStreams);
	SortStealCandidateList(m_stealStatics);
}

const SoundDesc* VoicePool::GetDesc(VoiceHandle vh) const noexcept
{
	auto voice = m_voices.Get(vh);
	if (!voice) return nullptr;

	return &voice->desc;
}

const ISoundInstance* VoicePool::GetInstance(VoiceHandle vh) const noexcept
{
	auto voice = m_voices.Get(vh);
	return voice ? voice->instance : nullptr;
}

ISoundInstance* VoicePool::GetInstance(VoiceHandle vh) noexcept
{
	return const_cast<ISoundInstance*>(static_cast<const VoicePool*>(this)->GetInstance(vh));
}
