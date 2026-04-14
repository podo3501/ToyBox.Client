#include "pch.h"
#include "SoundRepository.h"
#include "IAudioBackend.h"
#include "ISoundBuffer.h"
#include "LoadedSound.h"
#include "Service/Asset/Assets/SoundTableAsset.h"
#include "Service/Asset/Assets/StaticSoundAsset.h"
#include "Service/Asset/Assets/StreamSoundAsset.h"

SoundRepository::~SoundRepository() = default;
SoundRepository::SoundRepository(IAudioBackend* audioBackend) :
    m_audioBackend{ audioBackend }
{}

SoundHandle SoundRepository::AcquireStaticSound(const StaticSoundDesc* desc,
    function<shared_ptr<StaticSoundAsset>(const filesystem::path&)> loader)
{
    return AcquireSoundInternal(desc, [this, desc, loader]() {
        return CreateStaticSoundBuffer(desc, loader);
        });
}

SoundHandle SoundRepository::AcquireStreamSound(const StreamSoundDesc* desc,
    function<shared_ptr<StreamSoundAsset>(const filesystem::path&)> loader)
{
    return AcquireSoundInternal(desc, [this, desc, loader]() {
        return CreateStreamSoundBuffer(desc, loader);
        });
}

SoundHandle SoundRepository::AcquireSoundInternal(auto* desc, auto&& createBuffer)
{
    shared_ptr<ISoundBuffer> sndBuffer;

    auto it = m_buffers.find(desc->filename);
    if (it != m_buffers.end())
        sndBuffer = it->second.lock();

    if (!sndBuffer)
    {
        sndBuffer = createBuffer();
        if (!sndBuffer) return SoundHandle::Invalid();

        m_buffers.insert_or_assign(desc->filename, sndBuffer);
    }

    return m_loadedSounds.Emplace(desc, sndBuffer);
}

shared_ptr<ISoundBuffer> SoundRepository::CreateStaticSoundBuffer(const StaticSoundDesc* desc,
    function<shared_ptr<StaticSoundAsset>(const filesystem::path&)> loader)
{
    auto staticBuffer = m_audioBackend->CreateStaticSoundBuffer();
    if (!staticBuffer) return nullptr;

    auto asset = loader(desc->filename);
    if (!asset) return nullptr;

    if (!staticBuffer->LoadFromAsset(move(asset))) return nullptr; //?!? CreateStaticSoundBuffer 할때 asset을 인자로 바로 넣어서 하면 코드량이 조금 줄어들겠지.
    return staticBuffer;
}

shared_ptr<ISoundBuffer> SoundRepository::CreateStreamSoundBuffer(const StreamSoundDesc* desc,
    function<shared_ptr<StreamSoundAsset>(const filesystem::path&)> loader)
{
    auto streamBuffer = m_audioBackend->CreateStreamSoundBuffer();
    if (!streamBuffer) return nullptr;

    auto asset = loader(desc->filename);
    if (!asset) return nullptr;

    if (!streamBuffer->LoadFromAsset(move(asset))) return nullptr;
    return streamBuffer;
}

const LoadedSound* SoundRepository::Find(SoundHandle h) const noexcept
{
    const LoadedSound* snd = m_loadedSounds.Find(h);
    if (!snd) return nullptr; 

    return snd;
}

bool SoundRepository::Remove(SoundHandle h) noexcept
{
    return m_loadedSounds.Remove(h);
}