#pragma once
#include "Core/Utils/Handle/HandlePool.h"
#include "SoundHandle.h"

struct IAudioBackend;
struct ISoundBuffer;
struct SoundDesc;
struct StaticSoundDesc;
struct StreamSoundDesc;
struct StaticSoundAsset;
struct StreamSoundAsset;
struct LoadedSound;
class SoundRepository
{
public:
	~SoundRepository();
	SoundRepository() = delete;
	SoundRepository(IAudioBackend* audioBackend);

	SoundHandle AcquireStaticSound(const StaticSoundDesc* desc,
		function<shared_ptr<StaticSoundAsset>(const filesystem::path&)> loader);
	SoundHandle AcquireStreamSound(const StreamSoundDesc* desc,
		function<shared_ptr<StreamSoundAsset>(const filesystem::path&)> loader);

	const LoadedSound* Find(SoundHandle h) const noexcept;
	bool Remove(SoundHandle h) noexcept;

private:
	SoundHandle AcquireSoundInternal(auto* desc, auto&& createBuffer);
	shared_ptr<ISoundBuffer> CreateStaticSoundBuffer(const StaticSoundDesc* desc,
		function<shared_ptr<StaticSoundAsset>(const filesystem::path&)> loader);
	shared_ptr<ISoundBuffer> CreateStreamSoundBuffer(const StreamSoundDesc* desc,
		function<shared_ptr<StreamSoundAsset>(const filesystem::path&)> loader);

	IAudioBackend* m_audioBackend{ nullptr };
	unordered_map<filesystem::path, weak_ptr<ISoundBuffer>> m_buffers;
	HandlePool<LoadedSound, SoundTag> m_loadedSounds; //빈 SoundTag 스트럭쳐를 넣어서 handle의 타입을 만든다.
};
