#include "pch.h"
#include "StaticSound.h"
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

	return true;
}

bool StaticSound::LoadSound(string_view soundID, Core::ByteBuffer fileBuffer, AudioGroupID groupID, float volume)
{
	auto buffer = make_unique<StaticSoundInstance>(m_mixer);
	ReturnIfFalse(buffer->LoadFromMemory(move(fileBuffer), groupID, volume));

	m_instances.insert({ string(soundID), move(buffer) });
	return true;
}

bool StaticSound::Unload(string_view soundID) noexcept
{
	auto it = m_instances.find(string(soundID));
	if (it == m_instances.end()) return false;

	m_instances.erase(it);
	return true;
}

void StaticSound::SetVolume(AudioGroupID groupID, float volume) noexcept
{
	for (auto& buffer : m_instances | views::values)
	{
		if (!buffer->IsPlaying()) continue;
		if (buffer->GetGroupID() != groupID) continue;

		buffer->SetVolume(volume);
	}
}

bool StaticSound::SetVolume(string_view soundID, float volume) noexcept
{
	auto it = m_instances.find(string(soundID));
	if (it == m_instances.end()) return false;

	it->second->SetVolume(volume);
	return true;
}

bool StaticSound::Play(string_view soundID) noexcept
{
	auto it = m_instances.find(string(soundID));
	if (it == m_instances.end()) return false;

	it->second->Play();
	return true;
}

PlayState StaticSound::GetState(string_view soundID) const noexcept
{
	auto it = m_instances.find(string(soundID));
	if (it == m_instances.end()) return PlayState::None;

	return it->second->IsPlaying() ? PlayState::Playing : PlayState::Stopped;
}