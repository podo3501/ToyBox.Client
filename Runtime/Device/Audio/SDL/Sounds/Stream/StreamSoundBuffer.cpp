#include "pch.h"
#include "StreamSoundBuffer.h"
#include "Platform/Resource/IResourceStream.h"

StreamSoundBuffer::StreamSoundBuffer() = default;
bool StreamSoundBuffer::AttachStream(unique_ptr<IResourceStream> stream)
{
	m_fileStream = move(stream);
	return true;
}

IResourceStream* StreamSoundBuffer::GetStream() noexcept
{
	return m_fileStream.get();
}
