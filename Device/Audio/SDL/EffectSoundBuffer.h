#pragma once
#include "SDL3/SDL_audio.h"

enum class AudioGroupID;
class EffectSoundBuffer
{
public:
	EffectSoundBuffer();
	bool LoadFromFile(const string& filename, AudioGroupID groupID, float volume);
	bool SetVolume(float volume);
	void Play();
	bool IsPlaying() const noexcept;
	void Update();
	AudioGroupID GetGroupID() const noexcept;
	
private:
	void ResetPlay() noexcept;
	void PushChunk();

	SDL_AudioSpec m_spec{};
	unique_ptr<Uint8, function<void(Uint8*)>> m_data;
	AudioGroupID m_groupID;
	Uint32 m_length{ 0 };
	Uint32 m_offset{ 0 };
	bool m_resumed{ false };
	SDL_AudioStream* m_stream{ nullptr };
};
