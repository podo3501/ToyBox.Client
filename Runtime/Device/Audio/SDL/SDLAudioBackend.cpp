#include "pch.h"
#include "SDLAudioBackend.h"
#include "SDL3/SDL_init.h"
#include "AudioFormat.h"
#include "Sounds/Normal/NormalSound.h"
#include "Sounds/Stream/StreamSound.h"
#include "Platform/Resource/IResourceStream.h"

SDLAudioBackend::~SDLAudioBackend()
{
	m_normalSound.reset();
	m_streamSound.reset();
	SDL_Quit();
}

SDLAudioBackend::SDLAudioBackend() :
	m_streamSound{ make_unique<StreamSound>() },
	m_normalSound{ make_unique<NormalSound>() }
{}

bool SDLAudioBackend::Initialize() noexcept
{
	bool isInit = SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO;
	if (isInit) return true;

	ReturnIfFalse(SDL_Init(SDL_INIT_AUDIO));
	ReturnIfFalse(m_normalSound->Initialize());
	ReturnIfFalse(m_streamSound->Initialize());

	return true;
}

bool SDLAudioBackend::LoadPreload(string_view soundID, Core::ByteBuffer buffer, AudioGroupID groupID, float volume)
{
	return m_normalSound->LoadSound(soundID, move(buffer), groupID, volume);
}

bool SDLAudioBackend::LoadStream(string_view soundID, unique_ptr<IResourceStream> stream, AudioGroupID groupID, float volume)
{
	return m_streamSound->LoadSound(soundID, move(stream), groupID, volume);
}

bool SDLAudioBackend::Unload(string_view soundID) noexcept
{
	return m_normalSound->Unload(soundID);
}

bool SDLAudioBackend::Play(string_view soundID) noexcept
{
	return m_normalSound->Play(soundID);
}

PlayState SDLAudioBackend::GetState(string_view soundID) const noexcept
{
	return m_normalSound->GetState(soundID);
}

void SDLAudioBackend::Update() noexcept
{
	return;
}

//////////////////////////////////////////////////////

unique_ptr<IAudioBackend> CreateAudioBackend()
{
	auto audioBackend = make_unique<SDLAudioBackend>();
	if (!audioBackend->Initialize()) return nullptr;

	return audioBackend;
}