#include "pch.h"
#include "SDLAudioManager.h"
#include "Sounds/Effect/EffectSound.h"
#include "Sounds/Normal/NormalSound.h"
#include "../SoundInfo.h"
#include "SDL3/SDL_init.h"
#include "Platform/Framework/EnvironmentLocator.h"

struct AudioGroup
{
	float volume{ 1.f };
};

SDLAudioManager::~SDLAudioManager() 
{ 
	m_normalSound.reset();
	m_effectSound.reset();
	SDL_Quit();
}

SDLAudioManager::SDLAudioManager(SoundTable sndTable) :
	m_sndTable { move(sndTable) },
	m_effectSound{ make_unique<EffectSound>() },
	m_normalSound{ make_unique<NormalSound>() }
{}

bool SDLAudioManager::Initialize()
{
	bool isInit = SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO;
	if (isInit) return true;

	ReturnIfFalse(SDL_Init(SDL_INIT_AUDIO));
	ReturnIfFalse(m_effectSound->Initialize());
	ReturnIfFalse(m_normalSound->Initialize());
	CreateAudioGroup();
	return true;
}

void SDLAudioManager::CreateAudioGroup() noexcept
{
	for (int id : views::iota(0, static_cast<int>(AudioGroupID::None)))
		m_audioGroups[static_cast<AudioGroupID>(id)] = make_unique<AudioGroup>();
}

static bool IsWav(const filesystem::path& filename)
{
	string lower = filename.string();
	ranges::transform(lower, lower.begin(), [](unsigned char c) { return tolower(c); });

	return lower.ends_with(".wav");
}

bool SDLAudioManager::LoadSound(const string& index)
{
	auto info = m_sndTable.GetInfo(index);
	if (!info) return false;

	const auto& filename = GetResourceFullFilename(info->filename);
	auto groupID = info->groupID;
	auto volume = GetVolume(groupID);

	if (IsWav(filename))
		return m_effectSound->LoadWav(filename.string(), groupID, volume);
	else
		return m_normalSound->LoadSound(filename.string(), groupID, volume);
}

bool SDLAudioManager::Unload(const string& index) noexcept
{
	const auto filename = GetFullFilename(index);
	if (filename.empty()) return false;

	return m_effectSound->Unload(filename) || m_normalSound->Unload(filename.string());
}

void SDLAudioManager::SetVolume(AudioGroupID groupID, float volume) noexcept
{
	auto& audioGroup = m_audioGroups[groupID];
	audioGroup->volume = volume;

	m_effectSound->SetVolume(groupID, volume);
	m_normalSound->SetVolume(groupID, volume);
}

float SDLAudioManager::GetVolume(AudioGroupID groupID) const noexcept
{
	float groupVolume = m_audioGroups.at(groupID)->volume;
	float volume = m_masterVolume * groupVolume;

	return std::clamp(volume, 0.f, 1.f);
}

bool SDLAudioManager::Play(const string& index)
{
	const auto filename = GetFullFilename(index);
	if (filename.empty()) return false;

	return m_effectSound->Play(filename) || m_normalSound->Play(filename.string());
}

PlayState SDLAudioManager::GetPlayState(const string& index)
{
	const auto filename = GetFullFilename(index);
    if (filename.empty()) return PlayState::NotLoaded;

	PlayState esState = m_effectSound->GetPlayState(filename);
	if (esState != PlayState::NotLoaded) return esState;

	return m_normalSound->GetState(filename.string());
}

filesystem::path SDLAudioManager::GetFullFilename(const string& index) const noexcept
{
	auto info = m_sndTable.GetInfo(index);
	if (!info) return "";

	return GetResourceFullFilename(info->filename);
}

void SDLAudioManager::Update() noexcept
{
	m_effectSound->Update();
}

unique_ptr<IAudioManager> CreateAudioManager(SoundTable sndTable)
{
	auto audioManager = make_unique<SDLAudioManager>(move(sndTable));
	if (!audioManager->Initialize()) return nullptr;

	return audioManager;
}