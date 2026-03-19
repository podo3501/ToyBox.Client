#pragma once

struct IAudioBackend;
struct IResourceManager;
struct ISoundBuffer;
struct SoundDescriptor;
struct StaticSoundDescriptor;
struct StreamSoundDescriptor;

struct LoadedSound
{
	const SoundDescriptor* desc;
	shared_ptr<ISoundBuffer> buffer;
};

class SoundRepository
{
public:
	~SoundRepository();
	SoundRepository() = delete;
	SoundRepository(IAudioBackend* audioBackend, IResourceManager* resManager);
	int AcquireStaticSound(const StaticSoundDescriptor* desc);
	int AcquireStreamSound(const StreamSoundDescriptor* desc);
	const LoadedSound* Find(int soundHandle) const noexcept;
	bool Remove(int soundHandle) noexcept;

private:
	shared_ptr<ISoundBuffer> CreateStaticSoundBuffer(const StaticSoundDescriptor* desc);
	shared_ptr<ISoundBuffer> CreateStreamSoundBuffer(const StreamSoundDescriptor* desc);

	IAudioBackend* m_audioBackend{ nullptr };
	IResourceManager* m_resManager{ nullptr };
	unordered_map<filesystem::path, weak_ptr<ISoundBuffer>> m_buffers;
	unordered_map<int, LoadedSound> m_loadedSounds;
	int m_nextSoundHandle{ 1 };
};
