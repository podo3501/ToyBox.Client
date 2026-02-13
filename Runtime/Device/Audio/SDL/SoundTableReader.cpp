#include "pch.h"
#include "SoundTableReader.h"
#include "SoundInfo.h"
#include "Shared/Framework/EnvironmentLocator.h"
#include "Device/Storage/JsonObjectIO.h"

SoundTableReader::~SoundTableReader() = default;
SoundTableReader::SoundTableReader(unique_ptr<IJsonStorage> storage) :
	m_storage{ move(storage) }
{}

bool SoundTableReader::Read()
{
	return JsonObjectIO::Read<StorageKey::Resource>(*this, m_storage.get());
}

SoundInfo* SoundTableReader::GetInfo(const string& index) noexcept
{
	auto it = m_infos.find(index);
	if (it == m_infos.end()) return nullptr;

	return &(it->second);
}

void SoundTableReader::Serialize(Serializer& serializer)
{
	serializer.Process("Infos", m_infos);
}