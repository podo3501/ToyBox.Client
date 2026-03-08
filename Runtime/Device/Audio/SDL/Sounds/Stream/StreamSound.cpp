#include "pch.h"
#include "StreamSound.h"
#include "StreamSoundBuffer.h"
#include "Device/Audio/AudioTypes.h"
#include "Platform/Resource/IResourceStream.h"

StreamSound::~StreamSound()
{
	m_streamSoundBuffers.clear();
    SDL_CloseAudioDevice(m_device);
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}
StreamSound::StreamSound() = default;

bool StreamSound::Initialize()
{
	ReturnIfFalse(SDL_InitSubSystem(SDL_INIT_AUDIO));

    SDL_AudioSpec deviceSpec{};
    deviceSpec.freq = 48000;
    deviceSpec.format = SDL_AUDIO_S16;
    deviceSpec.channels = 2;

    m_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &deviceSpec);
    if (!m_device) return false;

    return true;
}

bool StreamSound::LoadSound(string_view soundID, unique_ptr<IResourceStream> stream, AudioGroupID groupID, float volume, bool loop)
{
    auto sndBuffer = make_unique<StreamSoundBuffer>();
	ReturnIfFalse(sndBuffer->Load(m_device, move(stream), groupID, volume, loop));

    m_streamSoundBuffers.emplace(string(soundID), move(sndBuffer));
	return true;
}

bool StreamSound::Play(string_view soundID) noexcept
{
    auto it = m_streamSoundBuffers.find(string(soundID));
    if (it == m_streamSoundBuffers.end()) return false;

    it->second->Play();
    return true;
}

PlayState StreamSound::GetState(string_view soundID) const noexcept
{
    auto it = m_streamSoundBuffers.find(string(soundID));
    if (it == m_streamSoundBuffers.end()) return PlayState::None;

    return it->second->IsPlaying() ? PlayState::Playing : PlayState::Stopped;
}

void StreamSound::Update() noexcept
{
    for (auto& buffer : m_streamSoundBuffers | views::values)
    {
        if (!buffer->IsPlaying()) continue;
        buffer->Update();
    }
}
