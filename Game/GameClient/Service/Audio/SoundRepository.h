#pragma once
#include "Core/Utils/Handle/HandlePool.h"
#include "Core/Foundation/ResourceID.h"
#include "SoundHandle.h"

struct IAudioBackend;
struct ISoundBuffer;
struct SoundDesc;
struct StaticSoundDesc;
struct StreamSoundDesc;
struct StaticSoundAsset;
struct StreamSoundAsset;
struct LoadedSound;
struct PendingSoundRequest;
struct IAssetAsyncLoader;

class SoundRepository
{
public:
	~SoundRepository();
	SoundRepository() = delete;
	SoundRepository(IAudioBackend* audioBackend, IAssetAsyncLoader* asyncLoader);

	SoundHandle AcquireStaticSound(const StaticSoundDesc* desc);
	SoundHandle AcquireStreamSound(const StreamSoundDesc* desc);
	void Update();

	const LoadedSound* Find(SoundHandle h) const noexcept;
	bool Remove(SoundHandle h) noexcept;

private:
	IAudioBackend* m_audioBackend{ nullptr };
	IAssetAsyncLoader* m_asyncLoader{ nullptr };
	unordered_map<Core::ResourceID, weak_ptr<ISoundBuffer>> m_buffers;
	HandlePool<LoadedSound, SoundTag> m_loadedSounds; //빈 SoundTag 스트럭쳐를 넣어서 handle의 타입을 만든다.

	std::vector<PendingSoundRequest> m_pending;
};
