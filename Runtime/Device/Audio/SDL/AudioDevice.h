#pragma once
#include "SDL3/SDL.h"

struct StreamAudioRequestDevice;
class AudioDevice
{
public:
    ~AudioDevice();
    AudioDevice();
    bool Initialize(const StreamAudioRequestDevice& config);
    SDL_AudioStream* CreateDeviceStream(const SDL_AudioSpec& srcSpec) const;
    inline SDL_AudioDeviceID Get() const noexcept { return m_device; }

private:
    SDL_AudioDeviceID m_device{};
    SDL_AudioSpec m_spec{};
};
