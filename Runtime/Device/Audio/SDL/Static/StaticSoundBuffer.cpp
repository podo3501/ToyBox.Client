#include "pch.h"
#include "StaticSoundBuffer.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "GameClient/Service/Asset/Assets/StaticSoundAsset.h"
#include "Core/Utils/Cast.hpp"

StaticSoundBuffer::~StaticSoundBuffer()
{
    if (m_audio) MIX_DestroyAudio(m_audio);
}

StaticSoundBuffer::StaticSoundBuffer(MIX_Mixer* mixer) noexcept :
    m_mixer{ mixer },
    m_audio{ nullptr }
{}

bool StaticSoundBuffer::LoadFromAsset(shared_ptr<AssetData> asset)
{
    auto staticAsset = Core::Cast<StaticSoundAsset>(asset);
    if (!staticAsset)
        return false;

    if (m_audio)
    {
        MIX_DestroyAudio(m_audio);
        m_audio = nullptr;
    }

    SDL_AudioSpec spec{};
    spec.freq = staticAsset->sampleRate;
    spec.channels = staticAsset->channels;

    switch (staticAsset->format)
    {
    case SampleFormat::Int16: spec.format = SDL_AUDIO_S16; break;
    case SampleFormat::Float32: spec.format = SDL_AUDIO_F32; break;
    default: return false;
    }
    m_audio = MIX_LoadRawAudio(m_mixer, staticAsset->data.data(), staticAsset->data.size(), &spec);

    return m_audio != nullptr;
}
