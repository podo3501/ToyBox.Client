#include "pch.h"
#include "SoundRepository.h"
#include "IAudioBackend.h"
#include "ISoundBuffer.h"
#include "LoadedSound.h"
#include "Asset/SoundTableAsset.h"
#include "Asset/StaticSoundAsset.h"
#include "Asset/StreamSoundAsset.h"
#include "Service/AssetAsyncHelper.h"
#include "Service/IAssetAsyncLoader.h"

struct PendingSoundRequest
{
    SoundHandle handle;
    AssetRequestID requestId;
};

SoundRepository::~SoundRepository() = default;
SoundRepository::SoundRepository(IAudioBackend* audioBackend, IAssetAsyncLoader* asyncLoader) :
    m_audioBackend{ audioBackend },
    m_asyncLoader{ asyncLoader }
{}

SoundHandle SoundRepository::AcquireStaticSound(const StaticSoundDesc* desc)
{
    shared_ptr<ISoundBuffer> sndBuffer;

    auto it = m_buffers.find(desc->resID);
    if (it != m_buffers.end())
        sndBuffer = it->second.lock();

    if (!sndBuffer)
    {
        sndBuffer = m_audioBackend->CreateStaticSoundBuffer();
        if (!sndBuffer)
            return SoundHandle::Invalid();
    }

    auto handle = m_loadedSounds.Emplace(desc, sndBuffer, SoundLoadState::Pending);
    auto reqID = Asset::PushRequest<StaticSoundAsset>(m_asyncLoader, desc->resID);
    m_pending.push_back({ handle, reqID });

    return handle;
}

SoundHandle SoundRepository::AcquireStreamSound(const StreamSoundDesc* desc)
{
    shared_ptr<ISoundBuffer> sndBuffer;

    auto it = m_buffers.find(desc->resID);
    if (it != m_buffers.end())
        sndBuffer = it->second.lock();

    if (!sndBuffer)
    {
        sndBuffer = m_audioBackend->CreateStreamSoundBuffer();
        if (!sndBuffer)
            return SoundHandle::Invalid();
    }

    auto handle = m_loadedSounds.Emplace(desc, sndBuffer, SoundLoadState::Pending);
    auto reqID = Asset::PushRequest<StreamSoundAsset>(m_asyncLoader, desc->resID);
    m_pending.push_back({ handle, reqID });

    return handle;
}

void SoundRepository::Update()
{
    if (m_pending.empty()) return;

    for (auto it = m_pending.begin(); it != m_pending.end(); )
    {
        auto& req = *it;
        auto asset = m_asyncLoader->TakeResult(req.requestId);
        if (!asset)
        {
            ++it;
            continue;
        }

        auto sound = m_loadedSounds.Find(req.handle);
        if (!sound) // 이미 unload 되었거나 제거된 경우
        {
            it = m_pending.erase(it);
            continue;
        }

        auto& buffer = sound->buffer;
        bool result = buffer->LoadFromAsset(asset);
        sound->state = result ? SoundLoadState::Ready : SoundLoadState::Failed;

        it = m_pending.erase(it);
    }
}

const LoadedSound* SoundRepository::Find(SoundHandle h) const noexcept
{
    return m_loadedSounds.Find(h);
}

bool SoundRepository::Remove(SoundHandle h) noexcept
{
    for (auto it = m_pending.begin(); it != m_pending.end();) // pending request 정리
    {
        if (it->handle == h)
            it = m_pending.erase(it);
        else
            ++it;
    }

    return m_loadedSounds.Remove(h);
}