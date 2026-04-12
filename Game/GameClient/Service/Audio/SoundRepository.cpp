#include "pch.h"
#include "SoundRepository.h"
#include "IAudioBackend.h"
#include "ISoundBuffer.h"
#include "LoadedSound.h"
#include "Service/Asset/Assets/SoundTableAsset.h"
#include "Service/Asset/Assets/StaticSoundAsset.h"
#include "Service/Asset/Assets/StreamSoundAsset.h"
#include "Platform/Resource/IResourceManager.h"

SoundRepository::~SoundRepository() = default;
SoundRepository::SoundRepository(IAudioBackend* audioBackend, IResourceManager* resManager) :
    m_audioBackend{ audioBackend }, m_resManager{ resManager }
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
        if (!sndBuffer) return InvalidSoundHandle;

        m_buffers.insert_or_assign(desc->filename, sndBuffer);
    }

    SoundHandle handle = SoundHandle{ m_nextSoundHandle.value++ };
    m_loadedSounds.emplace(handle, LoadedSound{ desc, sndBuffer });

    return handle;
}

shared_ptr<ISoundBuffer> SoundRepository::CreateStaticSoundBuffer(const StaticSoundDesc* desc,
    function<shared_ptr<StaticSoundAsset>(const filesystem::path&)> loader)
{
    auto staticBuffer = m_audioBackend->CreateStaticSoundBuffer();
    if (!staticBuffer) return nullptr;

    auto asset = loader(desc->filename);
    if (!asset) return nullptr;

    if (!staticBuffer->LoadFromAsset(move(asset))) return nullptr;
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

//shared_ptr<ISoundBuffer> SoundRepository::CreateStreamSoundBuffer(const StreamSoundDesc* desc)
//{
//    auto streamBuffer = m_audioBackend->CreateStreamSoundBuffer();
//    if (!streamBuffer) return nullptr;
//
//    auto stream = m_resManager->CreateReadStream(desc->filename);
//    if (!stream) return nullptr;
//
//    if (!streamBuffer->AttachStream(move(stream))) return nullptr;
//    return streamBuffer;
//}

const LoadedSound* SoundRepository::Find(SoundHandle h) const noexcept
{
	auto it = m_loadedSounds.find(h);
	if (it == m_loadedSounds.end()) return nullptr;

	return &(it->second);
}

bool SoundRepository::Remove(SoundHandle h) noexcept
{
	auto it = m_loadedSounds.find(h);
	if (it == m_loadedSounds.end()) return false;

	m_loadedSounds.erase(it);
	return true;
}