#pragma once

struct IResourceStream;
struct IStreamSoundBuffer;
class AudioDevice;
class StreamSoundInstance;
enum class AudioGroupID;
enum class PlaybackState;
class StreamSound
{
public:
	~StreamSound();
	StreamSound();
	bool Initialize();
	unique_ptr<IStreamSoundBuffer> CreateStreamSoundBuffer();
	bool LoadSound(string_view soundID, unique_ptr<IResourceStream> stream, AudioGroupID groupID, float volume, bool loop);
	bool Unload(string_view soundID) noexcept;
	void SetVolume(AudioGroupID groupID, float volume) noexcept;
	bool SetVolume(string_view soundID, float volume) noexcept;
	bool Play(string_view soundID) noexcept;
	PlaybackState GetState(string_view soundID) const noexcept;
	void Update() noexcept;

private:
	unique_ptr<AudioDevice> m_device;
	unordered_map<string, unique_ptr<StreamSoundInstance>> m_instances;
};
