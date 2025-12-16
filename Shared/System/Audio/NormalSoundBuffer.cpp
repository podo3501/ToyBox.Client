#include "pch.h"
#include "NormalSoundBuffer.h"
#include "../Public/AudioTypes.h"
#include "SDL3_mixer/SDL_mixer.h"

NormalSoundBuffer::~NormalSoundBuffer()
{
	if (m_track) MIX_DestroyTrack(m_track);
	if (m_audio) MIX_DestroyAudio(m_audio);
}

NormalSoundBuffer::NormalSoundBuffer(MIX_Mixer* mixer) :
	m_mixer{ mixer },
	m_groupID{ AudioGroupID::None }
{}

bool NormalSoundBuffer::LoadFromFile(const string& filename, AudioGroupID groupID, float volume)
{
	m_audio = MIX_LoadAudio(m_mixer, filename.c_str(), true);
	if (!m_audio) return false;

	m_track = MIX_CreateTrack(m_mixer);
	ReturnIfFalse(MIX_SetTrackAudio(m_track, m_audio));

	m_groupID = groupID;
	SetVolume(volume);

	m_options = SDL_CreateProperties();
	SDL_SetNumberProperty(m_options, MIX_PROP_PLAY_LOOPS_NUMBER, 0);
	return true;
}

void NormalSoundBuffer::Play()
{
	MIX_PlayTrack(m_track, m_options);
}

bool NormalSoundBuffer::SetVolume(float volume)
{
	return MIX_SetTrackGain(m_track, volume);
}

bool NormalSoundBuffer::IsPlaying() const noexcept
{
	return MIX_TrackPlaying(m_track);
}

AudioGroupID NormalSoundBuffer::GetGroupID() const noexcept { return m_groupID; }