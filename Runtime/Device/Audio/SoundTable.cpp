#include "pch.h"
#include "SoundTable.h"
#include "SoundInfo.h"
#include "Device/Storage/JsonObjectIO.h"
#include "Platform/Framework/EnvironmentLocator.h"

const SoundInfo* SoundTable::GetInfo(string_view index) const noexcept
{
	auto it = m_infos.find(string(index));
	if (it == m_infos.end()) return nullptr;

	return &(it->second);
}

void SoundTable::Serialize(Serializer& serializer)
{
	serializer.Process("Infos", m_infos);
}