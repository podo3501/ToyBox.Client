#include "pch.h"
#include "SDLAudioBackend.h"
#include "SDL3/SDL_init.h"
#include "AudioFormat.h"
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

bool SDLAudioBackend::LoadStatic(string_view soundID, Core::ByteBuffer buffer, AudioGroupID groupID, float volume)
{
	return m_staticSound->LoadSound(soundID, move(buffer), groupID, volume);
}

bool SDLAudioBackend::LoadStream(string_view soundID, unique_ptr<IResourceStream> stream, AudioGroupID groupID, float volume, bool loop)
{
	return m_streamSound->LoadSound(soundID, move(stream), groupID, volume, loop);
}

bool SDLAudioBackend::Unload(string_view soundID) noexcept
{
	if(m_staticSound->Unload(soundID)) return true;
	if (m_streamSound->Unload(soundID)) return true;

	return false;
}

bool SDLAudioBackend::Play(string_view soundID) noexcept
{
	if (m_staticSound->Play(soundID)) return true;
	if (m_streamSound->Play(soundID)) return true;

	return false;
}

bool SDLAudioBackend::SetVolume(string_view soundID, float volume) noexcept
{
	if (m_staticSound->SetVolume(soundID, volume)) return true;
	if (m_streamSound->SetVolume(soundID, volume)) return true;

	return false;
}

PlayState SDLAudioBackend::GetState(string_view soundID) const noexcept
{
	if (auto state = m_staticSound->GetState(soundID); state != PlayState::None) return state;
	if (auto state = m_streamSound->GetState(soundID); state != PlayState::None) return state;

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