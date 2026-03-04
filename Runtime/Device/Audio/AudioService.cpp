#include "pch.h"
#include "AudioService.h"
#include "IAudioBackend.h"

AudioService::~AudioService() = default;
AudioService::AudioService(SoundTable sndTable, 
	unique_ptr<IAudioBackend> audioBackend, IResourceManager* resManager) noexcept
	: m_sndTable{ move(sndTable) },
	m_audioBackend{ move(audioBackend) },
	m_resManager{ resManager }
{}

bool AudioService::LoadSound(string_view index)
{
	auto info = m_sndTable.GetInfo(index);
	if (!info) return false;

	//const auto& filename = GetResourceFullFilename(info->filename);
	//auto groupID = info->groupID;
	//auto volume = GetVolume(groupID);

	//if (IsWav(filename))
	//	return m_effectSound->LoadWav(filename.string(), groupID, volume);
	//else
	//	return m_normalSound->LoadSound(filename.string(), groupID, volume);

	return true;
}