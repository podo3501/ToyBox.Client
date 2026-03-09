#include "pch.h"
#include "StreamSound.h"
#include "StreamSoundInstance.h"
#include "AudioDevice.h"
#include "Device/Audio/AudioTypes.h"
#include "Platform/Resource/IResourceStream.h"

StreamSound::~StreamSound()
{
    m_instances.clear();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
StreamSound::StreamSound() = default;

bool StreamSound::Initialize()
{
	ReturnIfFalse(SDL_InitSubSystem(SDL_INIT_AUDIO));

    m_device = make_unique<AudioDevice>();
    ReturnIfFalse(m_device->Initialize());

    return true;
}

bool StreamSound::LoadSound(string_view soundID, unique_ptr<IResourceStream> stream, AudioGroupID groupID, float volume, bool loop)
{
    auto instance = make_unique<StreamSoundInstance>();
	ReturnIfFalse(instance->Load(m_device.get(), move(stream), groupID, volume, loop));

    m_instances.emplace(string(soundID), move(instance));
	return true;
}

bool StreamSound::Unload(string_view soundID) noexcept
{
    auto it = m_instances.find(string(soundID));
    if (it == m_instances.end()) return false;

    m_instances.erase(it);
    return true;
}

void StreamSound::SetVolume(AudioGroupID groupID, float volume) noexcept
{
    for (auto& instance : m_instances | views::values)
    {
        if (!instance->IsPlaying()) continue;
        if (instance->GetGroupID() != groupID) continue;

        instance->SetVolume(volume);
    }
}

bool StreamSound::SetVolume(string_view soundID, float volume) noexcept
{
    auto it = m_instances.find(string(soundID));
    if (it == m_instances.end()) return false;

    it->second->SetVolume(volume);
    return true;
}

bool StreamSound::Play(string_view soundID) noexcept
{
    auto it = m_instances.find(string(soundID));
    if (it == m_instances.end()) return false;

    it->second->Play();
    return true;
}

PlayState StreamSound::GetState(string_view soundID) const noexcept
{
    auto it = m_instances.find(string(soundID));
    if (it == m_instances.end()) return PlayState::None;

    return it->second->IsPlaying() ? PlayState::Playing : PlayState::Stopped;
}

void StreamSound::Update() noexcept
{
    for (auto& instance : m_instances | views::values)
    {
        if (!instance->IsPlaying()) continue;
        instance->Update();
    }
}
