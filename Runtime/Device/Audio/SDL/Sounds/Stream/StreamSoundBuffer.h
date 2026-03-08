#pragma once
#include "SDL3/SDL.h"
#include "vorbis/vorbisfile.h"
#include <array>

struct IResourceStream;
enum class AudioGroupID;
class StreamSoundBuffer
{
public:
	~StreamSoundBuffer();
	StreamSoundBuffer();

	bool Load(SDL_AudioDeviceID audioDevice, unique_ptr<IResourceStream> stream, AudioGroupID groupID, float volume, bool loop);
	void Play();
	void Stop();
	void Update() noexcept;
	bool SetVolume(float volume);
	bool IsPlaying() const noexcept;

private:
	bool PushChunk();
	
	OggVorbis_File m_vorbisFile{};
	bool m_vorbisOpened{ false };

	unique_ptr<IResourceStream> m_fileStream;
	SDL_AudioStream* m_stream{ nullptr };

	bool m_loop{ false };
	bool m_finished{ false };

	std::array<char, 16384> m_decodeBuffer{};
};
