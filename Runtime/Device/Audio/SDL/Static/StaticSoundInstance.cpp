#include "pch.h"
#include "StaticSoundInstance.h"
#include "StaticSoundBuffer.h"
#include "SDL3_mixer/SDL_mixer.h"

using enum PlaybackState;

StaticSoundInstance::~StaticSoundInstance()
{
    if (m_track) MIX_DestroyTrack(m_track);
}
StaticSoundInstance::StaticSoundInstance() = default;

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
    return MIX_SetTrackAudio(m_track, buffer->GetAudio());
}

bool StaticSoundInstance::Reset(const PlaybackParams& params)
{
    ReturnIfFalse(MIX_StopTrack(m_track, 0));
    ReturnIfFalse(MIX_SetTrackPlaybackPosition(m_track, 0));
    ReturnIfFalse(SetVolume(params.volume));

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

bool StaticSoundInstance::Resume()
{
    return MIX_ResumeTrack(m_track);
}

bool StaticSoundInstance::Stop()
{
    ReturnIfFalse(MIX_StopTrack(m_track, 0));
    ReturnIfFalse(MIX_SetTrackPlaybackPosition(m_track, 0));

    return true;
}

void StaticSoundInstance::Update()
{}

bool StaticSoundInstance::SetVolume(float volume)
{
    return MIX_SetTrackGain(m_track, volume);
}

PlaybackState StaticSoundInstance::GetState() const noexcept
{
    if (m_track == nullptr) return EnumUtil::Invalid<PlaybackState>;
    if (MIX_TrackPlaying(m_track)) return PlaybackState::Playing;
    if (MIX_TrackPaused(m_track)) return PlaybackState::Paused;

    return PlaybackState::Stopped;
}
