#include "pch.h"
#include "AudioService.h"
#include "IAudioBackend.h"
#include "Platform/Resource/IResourceManager.h"

struct GroupInfo //지금은 볼륨 하나지만 조금씩 확장될 가능성이 크다.
{
	float volume{ 1.f };
};

AudioService::~AudioService() = default;
AudioService::AudioService(SoundTable sndTable, 
	unique_ptr<IAudioBackend> audioBackend, IResourceManager* resManager) noexcept
	: m_sndTable{ move(sndTable) },
	m_audioBackend{ move(audioBackend) },
	m_resManager{ resManager }
{
	CreateAudioGroup();
}

void AudioService::CreateAudioGroup() noexcept
{
	for (int id : views::iota(0, static_cast<int>(AudioGroupID::None)))
		m_groupInfos[static_cast<AudioGroupID>(id)] = make_unique<GroupInfo>();
}

bool AudioService::Load(string_view soundID)
{
	auto info = m_sndTable.GetInfo(soundID);
	if (!info) return false;

	if (info->loadMode == SoundLoadMode::Preload)
	{
		Core::ByteBuffer buffer;
		ReturnIfFalse(m_resManager->Read(info->filename, buffer));

		return m_audioBackend->LoadPreload(
			soundID,
			move(buffer),
			info->groupID,
			GetGroupVolume(info->groupID));
	}
	
	if (info->loadMode == SoundLoadMode::Stream)
	{
		auto stream = m_resManager->CreateReadStream(info->filename);
		if (!stream) return false;

		return m_audioBackend->LoadStream(
			soundID,
			move(stream),
			info->groupID,
			GetGroupVolume(info->groupID),
			false);
	}

	return false;
}

bool AudioService::Unload(string_view soundID) noexcept
{
	return m_audioBackend->Unload(soundID);
}

bool AudioService::Play(string_view soundID) noexcept
{
	return m_audioBackend->Play(soundID);
}

PlayState AudioService::GetState(string_view soundID) const noexcept
{
	return m_audioBackend->GetState(soundID);
}

void AudioService::Update() noexcept
{
	m_audioBackend->Update();
}

bool AudioService::SetVolume(string_view soundID, float volume) noexcept
{
	auto groupID = GetGroupID(soundID);
	if (groupID == AudioGroupID::None) return false;

	float curVolume = GetGroupVolume(groupID) * volume;
	return m_audioBackend->SetVolume(soundID, curVolume);
}

float AudioService::GetGroupVolume(AudioGroupID groupID) const noexcept
{
	float groupVolume = m_groupInfos.at(groupID)->volume;
	float volume = m_masterVolume * groupVolume;

	return std::clamp(volume, 0.f, 1.f);
}

AudioGroupID AudioService::GetGroupID(string_view soundID)
{
	auto info = m_sndTable.GetInfo(soundID);
	if (!info) return AudioGroupID::None;

	return info->groupID;
}