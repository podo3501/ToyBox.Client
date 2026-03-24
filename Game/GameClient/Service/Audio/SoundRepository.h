#pragma once
#include "SoundHandle.h"

struct IAudioBackend;
struct IResourceManager;
struct ISoundBuffer;
struct SoundDescriptor;
struct StaticSoundDescriptor;
struct StreamSoundDescriptor;
struct LoadedSound;
class SoundRepository
{
public:
	~SoundRepository();
	SoundRepository() = delete;
	SoundRepository(IAudioBackend* audioBackend, IResourceManager* resManager);
	SoundHandle AcquireStaticSound(const StaticSoundDescriptor* desc);
	SoundHandle AcquireStreamSound(const StreamSoundDescriptor* desc);
	const LoadedSound* Find(SoundHandle h) const noexcept;
	bool Remove(SoundHandle h) noexcept;

private:
	SoundHandle AcquireSoundInternal(auto* desc, auto&& createBuffer);
	shared_ptr<ISoundBuffer> CreateStaticSoundBuffer(const StaticSoundDescriptor* desc);
	shared_ptr<ISoundBuffer> CreateStreamSoundBuffer(const StreamSoundDescriptor* desc);

	IAudioBackend* m_audioBackend{ nullptr };
	IResourceManager* m_resManager{ nullptr };
	unordered_map<filesystem::path, weak_ptr<ISoundBuffer>> m_buffers;
	unordered_map<SoundHandle, LoadedSound> m_loadedSounds;
	SoundHandle m_nextSoundHandle{ 1 }; // 0은 오류코드.
};
