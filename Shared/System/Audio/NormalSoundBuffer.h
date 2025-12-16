#pragma once
#include "SDL3/SDL_properties.h"

struct MIX_Mixer;
struct MIX_Audio;
struct MIX_Track;
enum class AudioGroupID;
class NormalSoundBuffer
{
public:
	~NormalSoundBuffer();
	NormalSoundBuffer() = delete;
	NormalSoundBuffer(MIX_Mixer* mixer);
	bool LoadFromFile(const string& filename, AudioGroupID groupID, float volume);
	void Play();
	bool SetVolume(float volume);
	bool IsPlaying() const noexcept;
	AudioGroupID GetGroupID() const noexcept;

private:
	MIX_Mixer* m_mixer{ nullptr };
	MIX_Audio* m_audio{ nullptr };
	MIX_Track* m_track{ nullptr };
	AudioGroupID m_groupID;
	SDL_PropertiesID m_options{};
};
