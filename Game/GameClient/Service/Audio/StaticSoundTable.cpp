#include "pch.h"
#include "StaticSoundTable.h"
#include "Device/Storage/JsonObjectIO.h"

const StaticSoundInfo* StaticSoundTable::GetInfo(string_view key) const noexcept
{
	auto it = m_infos.find(string(key));
	if (it == m_infos.end()) return nullptr;

	return &(it->second);
}

void StaticSoundTable::Serialize(Serializer& serializer)
{
	serializer.Process("Infos", m_infos);
}