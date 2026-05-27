#include "pch.h"
#include "SoundRepository.h"
#include "IAudioBackend.h"
#include "ISoundBuffer.h"
#include "LoadedSound.h"
#include "Service/Asset/Assets/SoundTableAsset.h"
#include "Service/Asset/Assets/StaticSoundAsset.h"
#include "Service/Asset/Assets/StreamSoundAsset.h"
#include "Service/AssetAsync/AssetPipeline.h"
#include "Core/Utils/Cast.hpp"

struct PendingSoundRequest
{
    SoundHandle handle;
    AssetRequestID requestId;
};

SoundRepository::~SoundRepository() = default;
SoundRepository::SoundRepository(IAudioBackend* audioBackend, AssetPipelineT* assetPipeline) :
    m_audioBackend{ audioBackend },
    m_assetPipeline{ assetPipeline }
{}

SoundHandle SoundRepository::AcquireStaticSound(const StaticSoundDesc* desc)
{
    shared_ptr<ISoundBuffer> sndBuffer;

    auto it = m_buffers.find(desc->filename);
    if (it != m_buffers.end())
        sndBuffer = it->second.lock();

    if (!sndBuffer)
    {
        sndBuffer = m_audioBackend->CreateStaticSoundBuffer();
        if (!sndBuffer)
            return SoundHandle::Invalid();
    }

    auto handle = m_loadedSounds.Emplace(desc, sndBuffer, SoundLoadState::Pending);
    auto reqID = m_assetPipeline->PushRequest(MakeAssetRequest<StaticSoundAsset>(desc->filename));
    m_pending.push_back({ handle, reqID });

    return handle;
}

SoundHandle SoundRepository::AcquireStreamSound(const StreamSoundDesc* desc)
{
    shared_ptr<ISoundBuffer> sndBuffer;

    auto it = m_buffers.find(desc->filename);
    if (it != m_buffers.end())
        sndBuffer = it->second.lock();

    if (!sndBuffer)
    {
        sndBuffer = m_audioBackend->CreateStreamSoundBuffer();
        if (!sndBuffer)
            return SoundHandle::Invalid();
    }

    auto handle = m_loadedSounds.Emplace(desc, sndBuffer, SoundLoadState::Pending);
    auto reqID = m_assetPipeline->PushRequest(MakeAssetRequest<StreamSoundAsset>(desc->filename));
    m_pending.push_back({ handle, reqID });

    return handle;
}

void SoundRepository::Update()
{
    if (m_pending.empty()) return;

    for (auto it = m_pending.begin(); it != m_pending.end(); )
    {
        auto& req = *it;
        auto asset = m_assetPipeline->TakeResult(req.requestId);
        if (!asset.has_value())
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
        bool result = buffer->LoadFromAsset(*asset);
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