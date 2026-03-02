#include "pch.h"
#include "SoundTableReader.h"
#include "SoundInfo.h"
#include "Device/Storage/JsonObjectIO.h"
#include "Platform/Framework/EnvironmentLocator.h"

SoundTableReader::~SoundTableReader() = default;
SoundTableReader::SoundTableReader() = default;
const SoundInfo* SoundTableReader::GetInfo(const string& index) const noexcept
{
	auto it = m_infos.find(index);
	if (it == m_infos.end()) return nullptr;

	return &(it->second);
}

void SoundTableReader::Serialize(Serializer& serializer)
{
	serializer.Process("Infos", m_infos);
}