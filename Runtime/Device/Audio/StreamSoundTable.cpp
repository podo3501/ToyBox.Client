#include "pch.h"
#include "StreamSoundTable.h"
#include "Device/Storage/JsonObjectIO.h"

const StreamSoundInfo* StreamSoundTable::GetInfo(string_view index) const noexcept
{
	auto it = m_infos.find(string(index));
	if (it == m_infos.end()) return nullptr;

	return &(it->second);
}

void StreamSoundTable::Serialize(Serializer& serializer)
{
	serializer.Process("Infos", m_infos);
}