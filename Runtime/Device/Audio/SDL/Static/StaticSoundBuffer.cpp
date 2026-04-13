#include "pch.h"
#include "StaticSoundBuffer.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "GameClient/Service/Asset/Assets/StaticSoundAsset.h"

StaticSoundBuffer::~StaticSoundBuffer()
{
    if (m_audio) MIX_DestroyAudio(m_audio);
}

StaticSoundBuffer::StaticSoundBuffer(MIX_Mixer* mixer) noexcept :
    m_mixer{ mixer },
    m_audio{ nullptr }
{}

bool StaticSoundBuffer::LoadFromAsset(shared_ptr<StaticSoundAsset> asset)
{
    if (m_audio)
    {
        MIX_DestroyAudio(m_audio);
        m_audio = nullptr;
    }

    SDL_AudioSpec spec{};
    spec.freq = asset->sampleRate;
    spec.channels = asset->channels;

    switch (asset->format)
    {
    case SampleFormat::Int16: spec.format = SDL_AUDIO_S16; break;
    case SampleFormat::Float32: spec.format = SDL_AUDIO_F32; break;
    default: return false;
    }
    m_audio = MIX_LoadRawAudio(m_mixer, asset->data.data(), asset->data.size(), &spec);

    return m_audio != nullptr;
}
