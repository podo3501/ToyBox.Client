#include "pch.h"
#include "NormalSound.h"
#include "NormalSoundBuffer.h"
#include "../Public/AudioTypes.h"
#include "SDL3_mixer/SDL_mixer.h"

NormalSound::~NormalSound()
{
	m_normalSoundBuffers.clear();
	if (m_mixer) MIX_DestroyMixer(m_mixer);
	MIX_Quit();
}
NormalSound::NormalSound() = default;

bool NormalSound::Initialize()
{
	ReturnIfFalse(MIX_Init());
	m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
	if (!m_mixer)
		return false;

	return true;
}

bool NormalSound::LoadSound(const string& filename, AudioGroupID groupID, float volume)
{
	auto buffer = make_unique<NormalSoundBuffer>(m_mixer);
	ReturnIfFalse(buffer->LoadFromFile(filename, groupID, volume));

	m_normalSoundBuffers.insert({ filename, move(buffer) });
	return true;
}

bool NormalSound::Unload(const string& filename) noexcept
{
	auto it = m_normalSoundBuffers.find(filename);
	if (it == m_normalSoundBuffers.end()) return false;

	m_normalSoundBuffers.erase(it);
	return true;
}

void NormalSound::SetVolume(AudioGroupID groupID, float volume) noexcept
{
	for (auto& buffer : m_normalSoundBuffers | views::values)
	{
		if (!buffer->IsPlaying()) continue;
		if (buffer->GetGroupID() != groupID) continue;

		buffer->SetVolume(volume);
	}
}

bool NormalSound::Play(const string& filename)
{
	auto it = m_normalSoundBuffers.find(filename);
	if (it == m_normalSoundBuffers.end()) return false;

	it->second->Play();
	return true;
}

PlayState NormalSound::GetPlayState(const string& filename) const noexcept
{
	auto it = m_normalSoundBuffers.find(filename);
	if (it == m_normalSoundBuffers.end()) return PlayState::NotLoaded;

	return it->second->IsPlaying() ? PlayState::Playing : PlayState::Stopped;
}