#include "pch.h"
#include "JsonLoader.h"
#include "GameClient/Service/Asset/AssetTypes.h"
#include "Platform/Serializer/Serializer.h"

StaticSoundDesc::StaticSoundDesc() :
	SoundDesc{ SoundType::Static }
{}

void StaticSoundDesc::Serialize(Serializer& serializer)
{
	SoundDesc::Serialize(serializer);

	//static 항목이 생기면 여기에 추가
}

//////////////////////////////////////////////////////////

const StaticSoundDesc* StaticSoundTable::GetDescriptor(string_view index) const noexcept
{
	auto it = m_descs.find(string(index));
	if (it == m_descs.end()) return nullptr;

	return &(it->second);
}

void StaticSoundTable::Serialize(Serializer& serializer)
{
	serializer.Process("Descriptors", m_descs);
}

std::unique_ptr<IAssetLoader> CreateStaticSoundTableLoader()
{
	return CreateJsonLoader<StaticSoundTable>();
}