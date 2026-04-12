#include "pch.h"
#include "StreamSoundBuffer.h"
#include "Platform/Resource/IResourceStream.h"
#include "GameClient/Service/Asset/Assets/StreamSoundAsset.h"

StreamSoundBuffer::StreamSoundBuffer() = default;
bool StreamSoundBuffer::AttachStream(unique_ptr<IResourceStream> stream)
{
	m_fileStream = move(stream);
	return true;
}

bool StreamSoundBuffer::LoadFromAsset(shared_ptr<StreamSoundAsset> asset)
{
	m_fileStream = move(asset->stream);
	return true;
}

IResourceStream* StreamSoundBuffer::GetStream() noexcept
{
	return m_fileStream.get();
}
