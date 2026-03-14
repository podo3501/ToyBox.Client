#include "pch.h"
#include "StaticSoundInstance.h"
#include "StaticSoundBuffer.h"
#include "Audio/AudioTypes.h"
#include "SDL3_mixer/SDL_mixer.h"

StaticSoundInstance::~StaticSoundInstance()
{
    if (m_track) MIX_DestroyTrack(m_track);
}

StaticSoundInstance::StaticSoundInstance() :
    m_buffer{ nullptr },
    m_track{ nullptr }
{}

bool StaticSoundInstance::Setup(MIX_Mixer* mixer)
{
    m_track = MIX_CreateTrack(mixer);
    if (!m_track) return false;

    m_options = SDL_CreateProperties();
    if (!m_options) return false;

    return true;
}

bool StaticSoundInstance::SetBuffer(StaticSoundBuffer* buffer)
{
    m_buffer = buffer;
    return MIX_SetTrackAudio(m_track, buffer->GetAudio());
}

bool StaticSoundInstance::Reset(float volume)
{
    ReturnIfFalse(MIX_StopTrack(m_track, 0));
    ReturnIfFalse(MIX_SetTrackPlaybackPosition(m_track, 0));
    ReturnIfFalse(SetVolume(volume));

    return true;
}

bool StaticSoundInstance::Play()
{
    return MIX_PlayTrack(m_track, m_options);
}

bool StaticSoundInstance::Pause()
{
    return MIX_PauseTrack(m_track);
}

bool StaticSoundInstance::Stop()
{
    if (!m_track) return false;

    ReturnIfFalse(MIX_StopTrack(m_track, 0));
    ReturnIfFalse(MIX_SetTrackPlaybackPosition(m_track, 0));

    return true;
}

bool StaticSoundInstance::SetVolume(float volume)
{
    return MIX_SetTrackGain(m_track, volume);
}

bool StaticSoundInstance::IsPlaying() const noexcept
{
    return MIX_TrackPlaying(m_track);
}

ISoundBuffer* StaticSoundInstance::GetBuffer() noexcept { return m_buffer; }