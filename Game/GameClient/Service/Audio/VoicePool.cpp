#include "pch.h"
#include "VoicePool.h"
#include "ISoundInstance.h"

static void SortStealCandidateList(vector<Voice*>& stealCandidates)
{
	ranges::sort(stealCandidates, [](Voice* lhs, Voice* rhs) {
		if (lhs->desc.priority != rhs->desc.priority) return lhs->desc.priority < rhs->desc.priority; //낮은 priority 부터 없앤다.
		return lhs->playbackTime > rhs->playbackTime ||
			(lhs->playbackTime == rhs->playbackTime && lhs < rhs); // 오래된 재생 먼저 없앤다.
		});
}

VoicePool::~VoicePool() = default;
VoicePool::VoicePool() = default;

bool VoicePool::Setup(int maxVoices, int maxStreams) noexcept
{
	if (maxVoices < 0 || maxStreams < 0) return false;
	if (maxVoices < maxStreams) return false;

	m_allocator = HandleAllocator<VoiceTag, 64>(maxVoices);
	m_voices.resize(maxVoices);
	m_maxStreams = maxStreams;

	return true;
}

VoiceHandle VoicePool::AcquireVoice(SoundHandle sh, ISoundInstance* instance, const SoundDescriptor* desc) noexcept
{
	auto vh = m_allocator.Acquire();

	if (!vh)
	{
		vector<Voice*> temp;
		auto& candidates = m_stealCandidates.empty() ? temp : m_stealCandidates; //steal 후보가 없으면 즉석해서 만든다.

		if (m_stealCandidates.empty())
		{
			temp.reserve(m_voices.size());
			for (auto& voice : m_voices)
			{
				if (!voice.instance) continue;
				temp.emplace_back(&voice);
			}

			SortStealCandidateList(temp);
		}

		for (auto* voice : candidates)
		{
			if (!voice->instance) continue;
			if (voice->desc.priority >= desc->priority) continue;

			StopVoice(voice->voiceHandle);
			vh = m_allocator.Acquire();
			if (vh) break;
		}

		if (!vh) return InvalidVoiceHandle;
	}

	uint16_t index = vh.Index();

	Voice& voice = m_voices[index];
	voice.voiceHandle = vh;
	voice.soundHandle = sh;
	voice.instance = instance;
	voice.desc = *desc;
	voice.playbackTime = 0;

	return vh;
}

bool VoicePool::StopVoice(VoiceHandle vh) noexcept
{
	if (!m_allocator.IsValid(vh)) return false;

	auto& voice = m_voices[vh.Index()];
	ReturnIfFalse(voice.StopAndReset());

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

void VoicePool::UpdateVoices() noexcept
{
	m_stealCandidates.clear();

	for (auto& voice : m_voices)
	{
		if (!voice.instance) continue;

		voice.instance->Update();
		
		auto state = voice.instance->GetState();
		if (state == PlaybackState::Playing)
		{
			voice.playbackTime += 1; // 재생 중이면 playbackTime값을 프레임 단위로 증가.
			m_stealCandidates.emplace_back(&voice);
		}
		else if (state == PlaybackState::Stopped)
			StopVoice(voice.voiceHandle);
	}

	SortStealCandidateList(m_stealCandidates);
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
