#include "pch.h"
#include "AudioMixer.h"
#include "SDL3_mixer/SDL_mixer.h"

AudioMixer::~AudioMixer() 
{
    if (m_mixer) MIX_DestroyMixer(m_mixer);
    MIX_Quit();
    SDL_Quit();
}

bool AudioMixer::Initialize() 
{
    bool isInit = SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO;
    if (isInit) return true;

    ReturnIfFalse(SDL_Init(SDL_INIT_AUDIO));
    ReturnIfFalse(MIX_Init());

    SDL_AudioSpec spec{}; //static sound 설정부분.
    spec.freq = 48000;
    spec.format = SDL_AUDIO_S16;
    spec.channels = 2;

    m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
    return m_mixer != nullptr;
}