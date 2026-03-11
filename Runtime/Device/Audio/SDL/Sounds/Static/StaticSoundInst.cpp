#include "pch.h"
#include "StaticSoundInst.h"
#include "StaticSoundBuffer.h"
#include "Audio/AudioTypes.h"
#include "SDL3_mixer/SDL_mixer.h"

StaticSoundInst::~StaticSoundInst()
{
    if (m_track) MIX_DestroyTrack(m_track);
}

StaticSoundInst::StaticSoundInst() :
    m_buffer{ nullptr },
    m_track{ nullptr },
    m_groupID{ AudioGroupID::None }
{}

bool StaticSoundInst::Setup(MIX_Mixer* mixer)
{
    m_track = MIX_CreateTrack(mixer);
    if (!m_track) return false;

    m_options = SDL_CreateProperties();
    if (!m_options) return false;

    return true;
}

bool StaticSoundInst::Reset(StaticSoundBuffer* buffer, AudioGroupID groupID, float volume)
{
    ReturnIfFalse(MIX_StopTrack(m_track, 0));
    ReturnIfFalse(MIX_SetTrackAudio(m_track, buffer->GetAudio()));
    ReturnIfFalse(MIX_SetTrackPlaybackPosition(m_track, 0));

    m_groupID = groupID;
    SetVolume(volume);

    return true;
}

void StaticSoundInst::Play()
{
    MIX_PlayTrack(m_track, m_options);
}

bool StaticSoundInst::SetVolume(float volume)
{
    return MIX_SetTrackGain(m_track, volume);
}

bool StaticSoundInst::IsPlaying() const noexcept
{
    return MIX_TrackPlaying(m_track);
}

AudioGroupID StaticSoundInst::GetGroupID() const noexcept { return m_groupID; }