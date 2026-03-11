#include "pch.h"
#include "SDLAudioBackend.h"
#include "SDL3/SDL_init.h"
#include "AudioFormat.h"
#include "../ISoundBuffer.h"
#include "Sounds/Static/StaticSound.h"
#include "Sounds/Stream/StreamSound.h"
#include "Platform/Resource/IResourceStream.h"

SDLAudioBackend::~SDLAudioBackend()
{
	m_staticSound.reset();
	m_streamSound.reset();
	SDL_Quit();
}

SDLAudioBackend::SDLAudioBackend() :
	m_staticSound{ make_unique<StaticSound>() },
	m_streamSound{ make_unique<StreamSound>() }
{}

bool SDLAudioBackend::Initialize() noexcept
{
	bool isInit = SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO;
	if (isInit) return true;

	ReturnIfFalse(SDL_Init(SDL_INIT_AUDIO));
	ReturnIfFalse(m_staticSound->Initialize());
	ReturnIfFalse(m_streamSound->Initialize());

	return true;
}

unique_ptr<IStaticSoundBuffer> SDLAudioBackend::CreateStaticSoundBuffer()
{
	return m_staticSound->CreateStaticSoundBuffer();
}

int SDLAudioBackend::CreateInstance(ISoundBuffer* sndBuffer, AudioGroupID groupID, float volume)
{
	return m_staticSound->CreateInstance(sndBuffer, groupID, volume);
}

bool SDLAudioBackend::LoadStream(string_view soundID, unique_ptr<IResourceStream> stream, AudioGroupID groupID, float volume, bool loop)
{
	return m_streamSound->LoadSound(soundID, move(stream), groupID, volume, loop);
}

bool SDLAudioBackend::Unload(string_view soundID) noexcept
{
	//if(m_staticSound->Unload(soundID)) return true;
	if (m_streamSound->Unload(soundID)) return true;

	return false;
}

bool SDLAudioBackend::Unload(int handle) noexcept
{
	if (m_staticSound->Unload(handle)) return true;

	return false;
}

bool SDLAudioBackend::Play(string_view soundID) noexcept
{
	//if (m_staticSound->Play(soundID)) return true;
	if (m_streamSound->Play(soundID)) return true;

	return false;
}

bool SDLAudioBackend::Play(int handle) noexcept
{
	if (m_staticSound->Play(handle)) return true;

	return false;
}

bool SDLAudioBackend::SetVolume(string_view soundID, float volume) noexcept
{
	//if (m_staticSound->SetVolume(soundID, volume)) return true;
	if (m_streamSound->SetVolume(soundID, volume)) return true;

	return false;
}

bool SDLAudioBackend::SetVolume(int handle, float volume) noexcept
{
	if (m_staticSound->SetVolume(handle, volume)) return true;

	return false;
}

PlayState SDLAudioBackend::GetState(string_view soundID) const noexcept
{
	//if (auto state = m_staticSound->GetState(soundID); state != PlayState::None) return state;
	if (auto state = m_streamSound->GetState(soundID); state != PlayState::None) return state;

	return PlayState::None;
}

PlayState SDLAudioBackend::GetState(int handle) const noexcept
{
	if (auto state = m_staticSound->GetState(handle); state != PlayState::None) return state;

	return PlayState::None;
}

void SDLAudioBackend::Update() noexcept
{
	return m_streamSound->Update();
}

//////////////////////////////////////////////////////

unique_ptr<IAudioBackend> CreateAudioBackend()
{
	auto audioBackend = make_unique<SDLAudioBackend>();
	if (!audioBackend->Initialize()) return nullptr;

	return audioBackend;
}