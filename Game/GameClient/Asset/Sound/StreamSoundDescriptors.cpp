#include "pch.h"
#include "StreamSoundDescriptors.h"
#include "Device/Storage/JsonObjectIO.h"

const StreamSoundDescriptor* StreamSoundDescriptors::GetDescriptor(string_view index) const noexcept
{
	auto it = m_descs.find(string(index));
	if (it == m_descs.end()) return nullptr;

	return &(it->second);
}

void StreamSoundDescriptors::Serialize(Serializer& serializer)
{
	serializer.Process("Descriptors", m_descs);
}