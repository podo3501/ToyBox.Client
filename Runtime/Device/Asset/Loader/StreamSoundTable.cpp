#include "pch.h"
#include "JsonLoader.h"
#include "GameClient/Asset/SoundTableAsset.h"
#include "Platform/Serializer/Serializer.h"

StreamSoundDesc::StreamSoundDesc() :
	SoundDesc{ SoundType::Stream }
{}

void StreamSoundDesc::Serialize(Serializer& serializer)
{
	SoundDesc::Serialize(serializer);

	serializer.Process("Loop", loop);
}

//////////////////////////////////////////////////////////

const StreamSoundDesc* StreamSoundTable::GetDescriptor(string_view index) const noexcept
{
	auto it = m_descs.find(string(index));
	if (it == m_descs.end()) return nullptr;

	return &(it->second);
}

void StreamSoundTable::Serialize(Serializer& serializer)
{
	serializer.Process("Descriptors", m_descs);
}

std::unique_ptr<IAssetLoader> CreateStreamSoundTableLoader()
{
	return CreateJsonLoader<StreamSoundTable>();
}