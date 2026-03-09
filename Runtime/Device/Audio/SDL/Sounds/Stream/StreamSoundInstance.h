#pragma once
#include "SDL3/SDL.h"
#include "vorbis/vorbisfile.h"
#include <array>

struct IResourceStream;
class AudioDevice;
enum class AudioGroupID;
class StreamSoundInstance
{
public:
	~StreamSoundInstance();
	StreamSoundInstance();

	bool Load(AudioDevice* device, unique_ptr<IResourceStream> stream, AudioGroupID groupID, float volume, bool loop);
	void Play();
	void Stop();
	void Update() noexcept;
	bool SetVolume(float volume);
	bool IsPlaying() const noexcept;
	AudioGroupID GetGroupID() const noexcept;

private:
	bool PushChunk();
	
	OggVorbis_File m_vorbisFile{};
	bool m_vorbisOpened{ false };

	unique_ptr<IResourceStream> m_fileStream;
	SDL_AudioStream* m_stream{ nullptr };

	bool m_loop{ false };
	AudioGroupID m_groupID;
	bool m_finished{ false };

	std::array<char, 16384> m_decodeBuffer{}; // 함수 한군데에서만 사용하지만 여기에 선언하는 이유는 멀티쓰레드나 멀티스트림이 되면 스택이 오버플로우 될수 있기 때문.
};
