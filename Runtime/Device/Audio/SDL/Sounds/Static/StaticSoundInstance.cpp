#include "pch.h"
#include "StaticSoundInstance.h"
#include "Audio/AudioTypes.h"
#include "SDL3_mixer/SDL_mixer.h"

StaticSoundInstance::~StaticSoundInstance()
{
	if (m_track) MIX_DestroyTrack(m_track);
	if (m_audio) MIX_DestroyAudio(m_audio);
}

StaticSoundInstance::StaticSoundInstance(MIX_Mixer* mixer) :
	m_mixer{ mixer },
	m_groupID{ AudioGroupID::None }
{}

bool StaticSoundInstance::LoadFromMemory(Core::ByteBuffer fileBuffer, AudioGroupID groupID, float volume)
{
	SDL_IOStream* io = SDL_IOFromConstMem(fileBuffer.data(), fileBuffer.size());
	m_audio = MIX_LoadAudio_IO(m_mixer, io, true, true);
	if (!m_audio) return false;

	m_track = MIX_CreateTrack(m_mixer);
	ReturnIfFalse(MIX_SetTrackAudio(m_track, m_audio));

	m_groupID = groupID;
	SetVolume(volume);

	m_options = SDL_CreateProperties();
	SDL_SetNumberProperty(m_options, MIX_PROP_PLAY_LOOPS_NUMBER, 0);
	return true;
}

void StaticSoundInstance::Play()
{
	MIX_PlayTrack(m_track, m_options);
}

bool StaticSoundInstance::SetVolume(float volume)
{
	return MIX_SetTrackGain(m_track, volume);
}

bool StaticSoundInstance::IsPlaying() const noexcept
{
	return MIX_TrackPlaying(m_track);
}

AudioGroupID StaticSoundInstance::GetGroupID() const noexcept { return m_groupID; }