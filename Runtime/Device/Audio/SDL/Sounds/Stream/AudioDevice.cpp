#include "pch.h"
#include "AudioDevice.h"

AudioDevice::~AudioDevice() { SDL_CloseAudioDevice(m_device); }
AudioDevice::AudioDevice() = default;
bool AudioDevice::Initialize()
{
    SDL_AudioSpec desired{};
    desired.freq = 48000;
    desired.format = SDL_AUDIO_F32;
    desired.channels = 2;

    m_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desired);
    if (m_device == 0)
        return false;

    return SDL_GetAudioDeviceFormat(m_device, &m_spec, nullptr);
}

SDL_AudioStream* AudioDevice::CreateStream(const SDL_AudioSpec& srcSpec)
{
    return SDL_CreateAudioStream(&srcSpec, &m_spec);
}

SDL_AudioDeviceID AudioDevice::GetDevice() const
{
    return m_device;
}