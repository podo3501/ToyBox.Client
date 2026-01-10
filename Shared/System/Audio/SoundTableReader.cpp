#include "pch.h"
#include "SoundTableReader.h"
#include "SoundTraits.h"
#include "AudioTypeHelpers.h"
#include "Shared/Framework/EnvironmentLocator.h"
#include "Shared/SerializerIO/JsonObjectIO.h"

void SoundInfo::ProcessIO(Serializer& serializer)
{
	serializer.Process("Filename", filename);
	serializer.Process("Group", groupID);
	serializer.Process("Volume", volume);
}

/////////////////////////////////////////////////////

SoundTableReader::~SoundTableReader() = default;
SoundTableReader::SoundTableReader(unique_ptr<IJsonStorage> storage) :
	m_storage{ move(storage) }
{}

bool SoundTableReader::Read(const wstring& filename)
{
	return JsonObjectIO::Load(*this, m_storage.get(), filename);
}

SoundInfo* SoundTableReader::GetInfo(const string& index) noexcept
{
	auto it = m_infos.find(index);
	if (it == m_infos.end()) return nullptr;

	return &(it->second);
}

void SoundTableReader::ProcessIO(Serializer& serializer)
{
	serializer.Process("Infos", m_infos);
}