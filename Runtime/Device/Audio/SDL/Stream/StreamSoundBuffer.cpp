#include "pch.h"
#include "StreamSoundBuffer.h"
#include "Platform/Resource/IResourceStream.h"
#include "GameClient/Asset/StreamSoundAsset.h"
#include "Core/Foundation/Cast.hpp"

StreamSoundBuffer::StreamSoundBuffer() = default;
bool StreamSoundBuffer::LoadFromAsset(shared_ptr<AssetData> asset)
{
	auto streamAsset = Core::Cast<StreamSoundAsset>(asset);
	if (!streamAsset)
		return false;

	m_fileStream = streamAsset->stream;
	return true;
}

IResourceStream* StreamSoundBuffer::GetStream() noexcept
{
	return m_fileStream.get();
}
