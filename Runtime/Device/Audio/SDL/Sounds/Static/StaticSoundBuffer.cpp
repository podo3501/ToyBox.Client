#include "pch.h"
#include "StaticSoundBuffer.h"
#include "SDL3_mixer/SDL_mixer.h"

StaticSoundBuffer::~StaticSoundBuffer()
{
    if (m_audio) MIX_DestroyAudio(m_audio);
}

StaticSoundBuffer::StaticSoundBuffer(MIX_Mixer* mixer) noexcept :
    m_mixer{ mixer },
    m_audio{ nullptr }
{}

bool StaticSoundBuffer::LoadFromMemory(Core::ByteBuffer fileBuffer)
{
    SDL_IOStream* io = SDL_IOFromConstMem(fileBuffer.data(), fileBuffer.size());
    m_audio = MIX_LoadAudio_IO(m_mixer, io, true, true);
    return m_audio != nullptr;
}
