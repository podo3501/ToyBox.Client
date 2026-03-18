#include "pch.h"
#include "StaticSoundInstance.h"
#include "StaticSoundBuffer.h"
#include "SDL3_mixer/SDL_mixer.h"

using enum PlaybackState;

StaticSoundInstance::~StaticSoundInstance()
{
    if (m_track) MIX_DestroyTrack(m_track);
    if (m_options) SDL_DestroyProperties(m_options);
}
StaticSoundInstance::StaticSoundInstance() = default;

bool StaticSoundInstance::Setup(MIX_Mixer* mixer)
{
    m_track = MIX_CreateTrack(mixer);
    if (!m_track) return false;

    m_options = SDL_CreateProperties();
    if (!m_options)
    {
        MIX_DestroyTrack(m_track);
        m_track = nullptr;
        return false;
    }

    return true;
}

bool StaticSoundInstance::SetBuffer(StaticSoundBuffer* buffer)
{
    if (!m_track || !buffer) return false;
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
    volume = clamp(volume, 0.0f, 1.0f);
    return MIX_SetTrackGain(m_track, volume);
}

PlaybackState StaticSoundInstance::GetState() const noexcept
{
    if (m_track == nullptr) return EnumUtil::Invalid<PlaybackState>;
    if (MIX_TrackPaused(m_track)) return PlaybackState::Paused;
    if (MIX_TrackPlaying(m_track)) return PlaybackState::Playing;

    return PlaybackState::Stopped;
}
