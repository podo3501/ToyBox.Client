#include "pch.h"
#include "SDLAudioBackend.h"
#include "SDL3/SDL_init.h"
#include "AudioFormat.h"
#include "Sounds/Effect/EffectSound.h"
#include "Sounds/Normal/NormalSound.h"

SDLAudioBackend::~SDLAudioBackend()
{
	m_normalSound.reset();
	m_effectSound.reset();
	SDL_Quit();
}

SDLAudioBackend::SDLAudioBackend() :
	m_effectSound{ make_unique<EffectSound>() },
	m_normalSound{ make_unique<NormalSound>() }
{}

bool SDLAudioBackend::Initialize() noexcept
{
	bool isInit = SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO;
	if (isInit) return true;

	ReturnIfFalse(SDL_Init(SDL_INIT_AUDIO));
	ReturnIfFalse(m_effectSound->Initialize());
	ReturnIfFalse(m_normalSound->Initialize());

	return true;
}

bool SDLAudioBackend::Load(string_view soundID, Core::ByteBuffer buffer, AudioGroupID groupID, float volume)
{
	auto audioFormat = DetectFormat(buffer);
	switch (audioFormat)
	{
	case AudioFormat::Wav: m_normalSound->LoadSound(soundID, move(buffer), groupID, volume);
	case AudioFormat::Ogg: m_normalSound->LoadSound(soundID, move(buffer), groupID, volume);
	}

	return true;
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