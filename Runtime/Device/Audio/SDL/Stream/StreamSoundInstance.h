#pragma once
#include "GameClient/Service/Audio/ISoundInstance.h"
#include "SDL3/SDL.h"
#include "vorbis/vorbisfile.h"
#include <array>

struct IResourceStream;
class AudioDevice;
class StreamSoundBuffer;
class StreamSoundInstance : public ISoundInstance
{
public:
	~StreamSoundInstance();
	StreamSoundInstance();

	virtual bool Reset(const PlaybackParams& params) override;
	virtual bool Play() override;
	virtual bool Pause() override;
	virtual bool Resume() override;
	virtual bool Stop() override;
	virtual void Update() override;
	virtual bool SetVolume(float volume) override;
	virtual PlaybackState GetState() const noexcept override;

	bool SetBuffer(StreamSoundBuffer* buffer);
	bool Setup(AudioDevice* device);

private:
	bool PrepareStream();
	bool PushChunk();
	
	AudioDevice* m_device{ nullptr };
	StreamSoundBuffer* m_buffer{ nullptr };
	unique_ptr<IResourceStream> m_resourceStream;
	OggVorbis_File m_vorbisFile{};
	bool m_vorbisOpened{ false };

	SDL_AudioStream* m_stream{ nullptr };
	float m_volume{ 1.f };
	bool m_loop{ false };
	bool m_draining{ false }; //데이터가 다 들어갔지만 큐에 소비할 것이 남아 있는지. 소비가 끝나야 Stopped로 전환된다.
	PlaybackState m_state{ PlaybackState::Stopped };

	std::array<char, 16384> m_decodeBuffer{}; // 함수 한군데에서만 사용하지만 여기에 선언하는 이유는 멀티쓰레드나 멀티스트림이 되면 스택이 오버플로우 될수 있기 때문.
};
