#include "pch.h"
#include "StaticSound.h"
#include "StaticSoundBuffer.h"
#include "StaticSoundInst.h"
#include "StaticSoundInstance.h"
#include "Audio/AudioTypes.h"
#include "SDL3_mixer/SDL_mixer.h"

StaticSound::~StaticSound()
{
	m_instances.clear();
	if (m_mixer) MIX_DestroyMixer(m_mixer);
	MIX_Quit();
}
StaticSound::StaticSound() = default;

bool StaticSound::Initialize()
{
	ReturnIfFalse(MIX_Init());
	m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
	if (!m_mixer)
		return false;

	constexpr int MaxVoices = 64;
	m_instances.resize(MaxVoices);

	for (auto& slot : m_instances)
	{
		ReturnIfFalse(slot.inst.Setup(m_mixer));
		slot.active = false;
	}

	return true;
}

unique_ptr<IStaticSoundBuffer> StaticSound::CreateStaticSoundBuffer()
{
	return make_unique<StaticSoundBuffer>(m_mixer);
}

int StaticSound::CreateInstance(ISoundBuffer* sndBuffer, AudioGroupID groupID, float volume)
{
	auto staticBuffer = static_cast<StaticSoundBuffer*>(sndBuffer);
	for (size_t i = 0; i < m_instances.size(); ++i)
	{
		auto& slot = m_instances[i];
		if (!slot.active)
		{
			if (!slot.inst.Reset(staticBuffer, groupID, volume))
				return 0;

			slot.active = true;
			return static_cast<int>(i + 1);
		}
	}

	return 0; // 미리 생성했던 풀이 다 사용중이다.
}

void StaticSound::SetVolume(AudioGroupID groupID, float volume) noexcept
{
	for (auto& slot : m_instances)
	{
		if (!slot.active) continue;
		if (slot.inst.GetGroupID() != groupID) continue;

		slot.inst.SetVolume(volume);
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
	auto slot = GetSlot(handle);
	if (slot == nullptr) return false;

	//slot->inst.Stop();
	slot->active = false;

	return true;
}

PlayState StaticSound::GetState(int handle) const noexcept
{
	auto instance = GetInstance(handle);
	if (instance == nullptr) return PlayState::None;

	return instance->IsPlaying() ? PlayState::Playing : PlayState::Stopped;
}

const InstanceSlot* StaticSound::GetSlot(int handle) const noexcept
{
	if (handle <= 0) return nullptr;

	size_t index = static_cast<size_t>(handle - 1);
	if (index >= m_instances.size()) return nullptr;

	auto& curSlot = m_instances[index];
	if (!curSlot.active) return nullptr;

	return &curSlot;
}

InstanceSlot* StaticSound::GetSlot(int handle) noexcept
{
	return const_cast<InstanceSlot*>(static_cast<const StaticSound*>(this)->GetSlot(handle));
}

const StaticSoundInst* StaticSound::GetInstance(int handle) const noexcept
{
	auto slot = GetSlot(handle);
	if (slot == nullptr) return nullptr;

	return &slot->inst;
}

StaticSoundInst* StaticSound::GetInstance(int handle) noexcept
{
	return const_cast<StaticSoundInst*>(static_cast<const StaticSound*>(this)->GetInstance(handle));
}