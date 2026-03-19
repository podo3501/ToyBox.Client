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

    MIX_SetTrackStoppedCallback(m_track, [](void* userdata, MIX_Track* track) {
        if (auto* self = static_cast<StaticSoundInstance*>(userdata))
            self->OnStopped();
        }, this);

    return true;
}

bool StaticSoundInstance::SetBuffer(StaticSoundBuffer* buffer)
{
    if (m_state == Playing) return false;
    if (!m_track || !buffer) return false;

    return MIX_SetTrackAudio(m_track, buffer->GetAudio());
}

bool StaticSoundInstance::Reset(const PlaybackParams& params)
{
    ReturnIfFalse(MIX_StopTrack(m_track, 0));
    ReturnIfFalse(MIX_SetTrackPlaybackPosition(m_track, 0));
    ReturnIfFalse(SetVolume(params.volume));
    m_state = Stopped;

    return true;
}

bool StaticSoundInstance::Play()
{
    ReturnIfFalse(MIX_PlayTrack(m_track, m_options));
    m_state = Playing;
    return true;
}

bool StaticSoundInstance::Pause()
{
    if (m_state != Playing) return false;

    ReturnIfFalse(MIX_PauseTrack(m_track));
    m_state = Paused;
    return true;
}

bool StaticSoundInstance::Resume()
{
    if (m_state != Paused) return false;

    ReturnIfFalse(MIX_ResumeTrack(m_track));
    m_state = Playing;
    return true;
}

bool StaticSoundInstance::Stop()
{
    return MIX_StopTrack(m_track, 0);
}

void StaticSoundInstance::OnStopped()
{
    auto isOk = MIX_SetTrackPlaybackPosition(m_track, 0);
    Assert(isOk);
    m_state = Stopped;
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
    return m_state;
}
