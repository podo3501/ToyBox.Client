#include "pch.h"
#include "StaticSoundDescriptors.h"
#include "Device/Storage/JsonObjectIO.h"

const StaticSoundDescriptor* StaticSoundDescriptors::GetDescriptor(string_view key) const noexcept
{
	auto it = m_descs.find(string(key));
	if (it == m_descs.end()) return nullptr;

	return &(it->second);
}

void StaticSoundDescriptors::Serialize(Serializer& serializer)
{
	serializer.Process("Descriptors", m_descs);
}