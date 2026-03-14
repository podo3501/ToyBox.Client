#include "pch.h"
#include "SDLAudioBackend.h"
#include "Static/StaticSoundBuffer.h"
#include "Sounds/Stream/StreamSound.h"
#include "Platform/Resource/IResourceStream.h"
#include "SDL3_mixer/SDL_mixer.h"

SDLAudioBackend::~SDLAudioBackend()
{
	m_streamSound.reset();
	if (m_mixer) MIX_DestroyMixer(m_mixer);
	MIX_Quit();
	SDL_Quit();
}

SDLAudioBackend::SDLAudioBackend() :
	m_streamSound{ make_unique<StreamSound>() }
{}

bool SDLAudioBackend::Initialize(int maxVoices) noexcept
{
	ReturnIfFalse(SetupAudioDevice());
	ReturnIfFalse(SetupStaticInstances(maxVoices));
	ReturnIfFalse(m_streamSound->Initialize());

	return true;
}

bool SDLAudioBackend::SetupAudioDevice() noexcept
{
	bool isInit = SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO;
	if (isInit) return true;

	ReturnIfFalse(SDL_Init(SDL_INIT_AUDIO));
	ReturnIfFalse(MIX_Init());
	m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
	if (!m_mixer) return false;
	
	return true;
}

bool SDLAudioBackend::SetupStaticInstances(int maxVoices) noexcept
{
	m_instances.resize(maxVoices);
	for (auto& instance : m_instances)
		ReturnIfFalse(instance.Setup(m_mixer));

	return true;
}

unique_ptr<IStaticSoundBuffer> SDLAudioBackend::CreateStaticSoundBuffer()
{
	return make_unique<StaticSoundBuffer>(m_mixer);
}

unique_ptr<IStreamSoundBuffer> SDLAudioBackend::CreateStreamSoundBuffer()
{
	return m_streamSound->CreateStreamSoundBuffer();
}

ISoundInstance* SDLAudioBackend::AcquireInstance(ISoundBuffer* sndBuffer, int index)
{
	if (sndBuffer->GetType() == SoundType::Static)
	{
		auto staticBuffer = static_cast<StaticSoundBuffer*>(sndBuffer);
		auto& instance = m_instances[index];
		if (!instance.SetBuffer(staticBuffer)) return nullptr;

		return &instance;
	}

	return nullptr;
}

bool SDLAudioBackend::LoadStream(string_view soundID, unique_ptr<IResourceStream> stream, AudioGroupID groupID, float volume, bool loop)
{
	return m_streamSound->LoadSound(soundID, move(stream), groupID, volume, loop);
}

void SDLAudioBackend::Update() noexcept
{
	return m_streamSound->Update();
}

//////////////////////////////////////////////////////

unique_ptr<IAudioBackend> CreateAudioBackend()
{
	return make_unique<SDLAudioBackend>();
}