#include "pch.h"
#include "SDLAudioBackend.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "Sounds/Stream/AudioDevice.h"
#include "Static/StaticSoundBuffer.h"
#include "Static/StaticSoundInstance.h"
#include "Sounds/Stream/StreamSoundBuffer.h"
#include "Sounds/Stream/StreamSoundInstance.h"
#include "Platform/Resource/IResourceStream.h"

SDLAudioBackend::~SDLAudioBackend()
{
	m_staticInstances.clear();
	m_streamInstances.clear();

	m_streamDevice.reset(); //stream 장치 먼저 삭제.
	if (m_mixer) MIX_DestroyMixer(m_mixer);
	MIX_Quit();
	SDL_Quit();
}
SDLAudioBackend::SDLAudioBackend() = default;

bool SDLAudioBackend::Initialize(int maxVoices, int maxStreams) noexcept
{
	ReturnIfFalse(SetupStaticAudioDevice());
	ReturnIfFalse(SetupStaticInstances(maxVoices));

	ReturnIfFalse(SetupStreamAudioDevice());
	ReturnIfFalse(SetupStreamInstances(maxStreams));

	return true;
}

bool SDLAudioBackend::SetupStaticAudioDevice() noexcept
{
	bool isInit = SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO;
	if (isInit) return true;

	ReturnIfFalse(SDL_Init(SDL_INIT_AUDIO));
	ReturnIfFalse(MIX_Init());
	m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
	if (!m_mixer) return false;
	
	return true;
}

bool SDLAudioBackend::SetupStreamAudioDevice() noexcept
{
	m_streamDevice = make_unique<AudioDevice>();
	ReturnIfFalse(m_streamDevice->Initialize());

	return true;
}

bool SDLAudioBackend::SetupStaticInstances(int maxVoices) noexcept
{
	m_staticInstances.resize(maxVoices);
	for (auto& instance : m_staticInstances)
		ReturnIfFalse(instance.Setup(m_mixer));

	return true;
}

bool SDLAudioBackend::SetupStreamInstances(int maxStreams) noexcept
{
	m_streamInstances.resize(maxStreams);
	for (auto& instance : m_streamInstances)
		ReturnIfFalse(instance.Setup(m_streamDevice.get()));

	return true;
}

unique_ptr<IStaticSoundBuffer> SDLAudioBackend::CreateStaticSoundBuffer()
{
	return make_unique<StaticSoundBuffer>(m_mixer);
}

unique_ptr<IStreamSoundBuffer> SDLAudioBackend::CreateStreamSoundBuffer()
{
	return make_unique<StreamSoundBuffer>();
}

ISoundInstance* SDLAudioBackend::AcquireInstance(SoundType type, ISoundBuffer* sndBuffer, int index)
{
	if (type == SoundType::Static)
	{
		auto staticBuffer = static_cast<StaticSoundBuffer*>(sndBuffer);
		auto& instance = m_staticInstances[index];
		if (!instance.SetBuffer(staticBuffer)) return nullptr;

		return &instance;
	}

	if (type == SoundType::Stream)
	{
		auto streamBuffer = static_cast<StreamSoundBuffer*>(sndBuffer);
		auto& instance = m_streamInstances[index];
		if (!instance.PrepareStream(streamBuffer)) return nullptr;

		return &instance;
	}

	return nullptr;
}

bool SDLAudioBackend::LoadStream(string_view soundID, unique_ptr<IResourceStream> stream, AudioGroupID groupID, float volume, bool loop)
{
	//return m_streamSound->LoadSound(soundID, move(stream), groupID, volume, loop);
	return true;
}

void SDLAudioBackend::Update() noexcept
{
	//return m_streamSound->Update();
}

//////////////////////////////////////////////////////

unique_ptr<IAudioBackend> CreateAudioBackend()
{
	return make_unique<SDLAudioBackend>();
}