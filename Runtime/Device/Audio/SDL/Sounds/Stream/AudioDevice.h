#pragma once
#include "SDL3/SDL.h"

class AudioDevice
{
public:
    ~AudioDevice();
    AudioDevice();
    bool Initialize();
    SDL_AudioStream* CreateStream(const SDL_AudioSpec& srcSpec);
    SDL_AudioDeviceID GetDevice() const;

private:
    SDL_AudioDeviceID m_device{};
    SDL_AudioSpec m_spec{};
};
