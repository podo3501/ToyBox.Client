#include "pch.h"
#include "StreamSoundBuffer.h"
#include "Platform/Resource/IResourceStream.h"
#include "GameClient/Service/Asset/Assets/StreamSoundAsset.h"
#include "Core/Utils/Cast.hpp"

StreamSoundBuffer::StreamSoundBuffer() = default;
bool StreamSoundBuffer::LoadFromAsset(shared_ptr<Asset> asset)
{
	auto streamAsset = Core::Cast<StreamSoundAsset>(asset);
	if (!streamAsset)
		return false;

	m_fileStream = move(streamAsset->stream);
	return true;
}

IResourceStream* StreamSoundBuffer::GetStream() noexcept
{
	return m_fileStream.get();
}
