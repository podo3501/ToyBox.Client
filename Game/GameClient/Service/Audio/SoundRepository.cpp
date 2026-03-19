#include "pch.h"
#include "SoundRepository.h"
#include "IAudioBackend.h"
#include "ISoundBuffer.h"
#include "Asset/Sound/Desc/StaticSoundDescriptor.h"
#include "Asset/Sound/Desc/StreamSoundDescriptor.h"
#include "Platform/Resource/IResourceManager.h"

SoundRepository::~SoundRepository() = default;
SoundRepository::SoundRepository(IAudioBackend* audioBackend, IResourceManager* resManager) :
    m_audioBackend{ audioBackend }, m_resManager{ resManager }
{}

int SoundRepository::AcquireStaticSound(const StaticSoundDescriptor* desc)
{
    std::shared_ptr<ISoundBuffer> sndBuffer;

    auto it = m_buffers.find(desc->filename);
    if (it != m_buffers.end())
        sndBuffer = it->second.lock();

    if (!sndBuffer)
    {
        sndBuffer = CreateStaticSoundBuffer(desc);
        if (!sndBuffer) return 0;

        m_buffers.insert_or_assign(desc->filename, sndBuffer);
    }

    int soundHandle = m_nextSoundHandle++;
    m_loadedSounds.emplace(soundHandle, LoadedSound{ desc, sndBuffer });

    return soundHandle;
}

int SoundRepository::AcquireStreamSound(const StreamSoundDescriptor* desc)
{
    std::shared_ptr<ISoundBuffer> sndBuffer;

    auto it = m_buffers.find(desc->filename);
    if (it != m_buffers.end())
        sndBuffer = it->second.lock();

    if (!sndBuffer)
    {
        sndBuffer = CreateStreamSoundBuffer(desc);
        if (!sndBuffer) return 0;

        m_buffers.insert_or_assign(desc->filename, sndBuffer);
    }

    int soundHandle = m_nextSoundHandle++;
    m_loadedSounds.emplace(soundHandle, LoadedSound{ desc, sndBuffer });

    return soundHandle;
}

shared_ptr<ISoundBuffer> SoundRepository::CreateStaticSoundBuffer(const StaticSoundDescriptor* desc)
{
    Core::ByteBuffer buffer;
    if (!m_resManager->Read(desc->filename, buffer)) return nullptr;

    auto staticBuffer = m_audioBackend->CreateStaticSoundBuffer();
    if (!staticBuffer) return nullptr;

    if (!staticBuffer->LoadFromMemory(move(buffer))) return nullptr;
    return staticBuffer;
}

shared_ptr<ISoundBuffer> SoundRepository::CreateStreamSoundBuffer(const StreamSoundDescriptor* desc)
{
    auto streamBuffer = m_audioBackend->CreateStreamSoundBuffer();
    if (!streamBuffer) return nullptr;

    auto stream = m_resManager->CreateReadStream(desc->filename);
    if (!stream) return nullptr;

    if (!streamBuffer->AttachStream(move(stream))) return nullptr;
    return streamBuffer;
}

const LoadedSound* SoundRepository::Find(int soundHandle) const noexcept
{
	auto it = m_loadedSounds.find(soundHandle);
	if (it == m_loadedSounds.end()) return nullptr;

	return &(it->second);
}

bool SoundRepository::Remove(int soundHandle) noexcept
{
	auto it = m_loadedSounds.find(soundHandle);
	if (it == m_loadedSounds.end()) return false;

	m_loadedSounds.erase(it);
	return true;
}