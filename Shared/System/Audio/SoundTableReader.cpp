#include "pch.h"
#include "SoundTableReader.h"
#include "SoundTraits.h"
#include "AudioTypeHelpers.h"
#include "Shared/Framework/EnvironmentLocator.h"
#include "Shared/SerializerIO/SerializerIO.h"

struct SoundInfoImpl : public SoundInfo
{
	void ProcessIO(SerializerIO& serializer)
	{
		serializer.Process("Filename", filename);
		serializer.Process("Group", groupID);
		serializer.Process("Volume", volume);
	}
};

/////////////////////////////////////////////////////

SoundTableReader::~SoundTableReader() = default;
SoundTableReader::SoundTableReader() = default;

bool SoundTableReader::Read(const wstring& filename)
{
	auto fullFilename = GetResourceFullFilenameW(filename);
	return SerializerIO::ReadJsonFromFile(fullFilename, *this);
}

SoundInfo* SoundTableReader::GetInfo(const string& index) noexcept
{
	auto it = m_infos.find(index);
	if (it == m_infos.end()) return nullptr;

	return &(it->second);
}

void SoundTableReader::ProcessIO(SerializerIO& serializer)
{
	serializer.Process("Infos", m_infos);
}

unique_ptr<ISoundTableReader> CreateSoundTableReader()
{
	return make_unique<SoundTableReader>();
}