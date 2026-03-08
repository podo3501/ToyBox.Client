#pragma once

struct IResourceStream;
class StreamSoundBuffer;
enum class AudioGroupID;
enum class PlayState;
class StreamSound
{
	using SDL_AudioDeviceID = uint32_t;

public:
	~StreamSound();
	StreamSound();
	bool Initialize();
	bool LoadSound(string_view soundID, unique_ptr<IResourceStream> stream, AudioGroupID groupID, float volume, bool loop);
	bool Play(string_view soundID) noexcept;
	PlayState GetState(string_view soundID) const noexcept;
	void Update() noexcept;

private:
	SDL_AudioDeviceID m_device{};
	unordered_map<string, unique_ptr<StreamSoundBuffer>> m_streamSoundBuffers;
};
