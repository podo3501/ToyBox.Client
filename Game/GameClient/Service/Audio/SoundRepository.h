#pragma once
#include "SoundHandle.h"

struct IAudioBackend;
struct IResourceManager;
struct ISoundBuffer;
struct SoundDesc;
struct StaticSoundDesc;
struct StreamSoundDesc;
struct LoadedSound;
class SoundRepository
{
public:
	~SoundRepository();
	SoundRepository() = delete;
	SoundRepository(IAudioBackend* audioBackend, IResourceManager* resManager);
	SoundHandle AcquireStaticSound(const StaticSoundDesc* desc);
	SoundHandle AcquireStreamSound(const StreamSoundDesc* desc);
	const LoadedSound* Find(SoundHandle h) const noexcept;
	bool Remove(SoundHandle h) noexcept;

private:
	SoundHandle AcquireSoundInternal(auto* desc, auto&& createBuffer);
	shared_ptr<ISoundBuffer> CreateStaticSoundBuffer(const StaticSoundDesc* desc);
	shared_ptr<ISoundBuffer> CreateStreamSoundBuffer(const StreamSoundDesc* desc);

	IAudioBackend* m_audioBackend{ nullptr };
	IResourceManager* m_resManager{ nullptr };
	unordered_map<filesystem::path, weak_ptr<ISoundBuffer>> m_buffers;
	unordered_map<SoundHandle, LoadedSound> m_loadedSounds;
	SoundHandle m_nextSoundHandle{ 1 }; // 0은 오류코드.
};
