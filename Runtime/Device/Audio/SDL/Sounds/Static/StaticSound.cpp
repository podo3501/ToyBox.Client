#include "pch.h"
#include "StaticSound.h"
#include "StaticSoundBuffer.h"
#include "StaticSoundInst.h"
#include "StaticSoundInstance.h"
#include "Audio/AudioTypes.h"
#include "SDL3_mixer/SDL_mixer.h"

StaticSound::~StaticSound()
{
	m_voices.clear();
	if (m_mixer) MIX_DestroyMixer(m_mixer);
	MIX_Quit();
}
StaticSound::StaticSound() = default;

bool StaticSound::Initialize(int maxVoices)
{
	ReturnIfFalse(MIX_Init());
	m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
	if (!m_mixer)
		return false;

	m_cycleIter.SetRange(0, maxVoices); //인덱스를 차례로 사용하게 하는 순환인덱스 생성.
	m_voices.resize(maxVoices);
	for (auto& voice : m_voices)
	{
		ReturnIfFalse(voice.inst.Setup(m_mixer));
		voice.active = false;
	}

	return true;
}

bool StaticSound::InitializE(int maxVoices)
{
	ReturnIfFalse(MIX_Init());
	m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
	if (!m_mixer)
		return false;

	m_instances.resize(maxVoices);
	for (auto& instance : m_instances)
		ReturnIfFalse(instance.Setup(m_mixer));

	return true;
}

unique_ptr<IStaticSoundBuffer> StaticSound::CreateStaticSoundBuffer()
{
	return make_unique<StaticSoundBuffer>(m_mixer);
}

ISoundInstance* StaticSound::AcquireInstance(StaticSoundBuffer* buffer, int index) noexcept
{
	auto& instance = m_instances[index];
	if(!instance.SetBuffer(buffer)) return nullptr;

	return &instance;
}

//int StaticSound::CreateInstance(ISoundBuffer* sndBuffer, AudioGroupID groupID, float volume)
//{
//	auto staticBuffer = static_cast<StaticSoundBuffer*>(sndBuffer);
//
//	for (size_t checked : views::iota(0u, m_voices.size()))
//	{
//		int index = m_cycleIter.Increase();
//		auto& voice = m_voices[index];
//
//		if (!voice.active)
//		{
//			if (!voice.inst.Reset(staticBuffer, groupID, volume))
//				return 0;
//
//			voice.active = true;
//			voice.generation++;
//			return (index & 0xFFFF) | (voice.generation << 16); // 32비트 핸들 생성: 하위 16비트 = 슬롯 인덱스, 상위 16비트 = 세대
//		}
//	}
//
//	return 0; // 풀 전체 사용중
//}

void StaticSound::SetVolume(AudioGroupID groupID, float volume) noexcept
{
	for (auto& voice : m_voices)
	{
		if (!voice.active) continue;
		if (voice.inst.GetGroupID() != groupID) continue;

		voice.inst.SetVolume(volume);
	}
}

bool StaticSound::SetVolume(int handle, float volume) noexcept
{
	auto instance = GetInstance(handle);
	if (instance == nullptr) return false;

	instance->SetVolume(volume);
	return true;
}

bool StaticSound::Unload(int handle) noexcept
{
	//auto it = m_instances.find(handle);
	//if (it == m_instances.end()) return false;

	//m_instances.erase(it);
	return true;
}

bool StaticSound::Play(int handle) noexcept
{
	auto instance = GetInstance(handle);
	if (instance == nullptr) return false;

	instance->Play();
	return true;
}

bool StaticSound::Stop(int handle) noexcept
{
	auto voice = GetVoice(handle);
	if (voice == nullptr) return false;

	ReturnIfFalse(voice->inst.Stop());
	voice->active = false;

	return true;
}

PlayState StaticSound::GetState(int handle) const noexcept
{
	auto instance = GetInstance(handle);
	if (instance == nullptr) return PlayState::None;

	return instance->IsPlaying() ? PlayState::Playing : PlayState::None;
}

const VoicE* StaticSound::GetVoice(int handle) const noexcept
{
	if (handle <= 0) return nullptr;

	// 핸들에서 슬롯 인덱스와 세대 추출
	size_t index = static_cast<size_t>(handle & 0xFFFF);
	uint16_t gen = static_cast<uint16_t>((handle >> 16) & 0xFFFF);
	if (index >= m_voices.size()) return nullptr;

	const auto& voice = m_voices[index];
	if (!voice.active || voice.generation != gen) // 슬롯이 비활성 상태거나 세대가 다르면 nullptr 반환
		return nullptr;

	return &voice;
}

VoicE* StaticSound::GetVoice(int handle) noexcept
{
	return const_cast<VoicE*>(static_cast<const StaticSound*>(this)->GetVoice(handle));
}

const StaticSoundInst* StaticSound::GetInstance(int handle) const noexcept
{
	auto voice = GetVoice(handle);
	if (voice == nullptr) return nullptr;

	return &voice->inst;
}

StaticSoundInst* StaticSound::GetInstance(int handle) noexcept
{
	return const_cast<StaticSoundInst*>(static_cast<const StaticSound*>(this)->GetInstance(handle));
}