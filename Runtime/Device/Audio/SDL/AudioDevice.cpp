#include "pch.h"
#include "AudioDevice.h"
#include "SDLAudioConfig.h"

AudioDevice::~AudioDevice() { SDL_CloseAudioDevice(m_device); }
AudioDevice::AudioDevice() = default;
bool AudioDevice::Initialize(const StreamAudioRequestDevice& config)
{
    SDL_AudioSpec desired{};
    desired.freq = config.freq;
    desired.format = config.format;
    desired.channels = config.channels;

    m_device = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &desired);
    if (m_device == 0)
        return false;

    return SDL_GetAudioDeviceFormat(m_device, &m_spec, nullptr);
}

SDL_AudioStream* AudioDevice::CreateDeviceStream(const SDL_AudioSpec& srcSpec) const
{
    auto stream = SDL_CreateAudioStream(&srcSpec, &m_spec);
    if(!SDL_BindAudioStream(m_device, stream)) return nullptr;

    return stream;
}