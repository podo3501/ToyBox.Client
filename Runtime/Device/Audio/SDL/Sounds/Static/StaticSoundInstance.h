#pragma once
#include "SDL3/SDL_properties.h"

struct MIX_Mixer;
struct MIX_Audio;
struct MIX_Track;
enum class AudioGroupID;
class StaticSoundInstance
{
public:
	~StaticSoundInstance();
	StaticSoundInstance() = delete;
	StaticSoundInstance(MIX_Mixer* mixer);
	bool LoadFromMemory(Core::ByteBuffer fileBuffer, AudioGroupID groupID, float volume);
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
