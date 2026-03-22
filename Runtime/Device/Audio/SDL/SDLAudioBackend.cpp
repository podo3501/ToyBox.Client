#include "pch.h"
#include "SDLAudioBackend.h"
#include "Static/StaticSoundBuffer.h"
#include "Static/StaticSoundInstance.h"
#include "Stream/StreamSoundBuffer.h"
#include "Stream/StreamSoundInstance.h"

SDLAudioBackend::~SDLAudioBackend() = default;
SDLAudioBackend::SDLAudioBackend() = default;
bool SDLAudioBackend::Initialize(int maxVoices, int maxStreams) noexcept
{
	if (maxVoices > 64 || maxStreams > 16) return false; //?!? 64와 16 이건 config 같은데로 constexpr 로 만들어 놓자.
	if (maxVoices < maxStreams) return false;
	
	ReturnIfFalse(m_mixer.Initialize());
	ReturnIfFalse(m_streamDevice.Initialize());
	ReturnIfFalse(SetupStaticInstances(maxVoices));
	ReturnIfFalse(SetupStreamInstances(maxStreams));

	return true;
}

bool SDLAudioBackend::SetupStaticInstances(int maxVoices) noexcept
{
	m_staticInstances.resize(maxVoices);
	for (auto& instance : m_staticInstances)
		ReturnIfFalse(instance.Setup(m_mixer.Get()));

	return true;
}

bool SDLAudioBackend::SetupStreamInstances(int maxStreams) noexcept
{
	m_streamInstances.reserve(maxStreams);
	for (int i = 0; i < maxStreams; ++i)
	{
		auto instance = make_unique<StreamSoundInstance>();
		if (!instance->Setup(&m_streamDevice))
			return false;

		m_streamInstances.emplace_back(move(instance));
	}

	return true;
}

unique_ptr<IStaticSoundBuffer> SDLAudioBackend::CreateStaticSoundBuffer()
{
	return make_unique<StaticSoundBuffer>(m_mixer.Get());
}

unique_ptr<IStreamSoundBuffer> SDLAudioBackend::CreateStreamSoundBuffer()
{
	return make_unique<StreamSoundBuffer>();
}

ISoundInstance* SDLAudioBackend::RequestStaticInstance(ISoundBuffer* sndBuffer)
{
	auto staticBuffer = static_cast<StaticSoundBuffer*>(sndBuffer);
	for (auto& instance : m_staticInstances)
	{
		if (instance.SetBuffer(staticBuffer))
			return &instance;
	}

	return nullptr;
}

ISoundInstance* SDLAudioBackend::RequestStreamInstance(ISoundBuffer* sndBuffer)
{
	auto streamBuffer = static_cast<StreamSoundBuffer*>(sndBuffer);
	for (auto& instance : m_streamInstances)
	{
		if (instance->SetBuffer(streamBuffer))
			return instance.get();
	}

	return nullptr;
}

//////////////////////////////////////////////////////

unique_ptr<IAudioBackend> CreateAudioBackend()
{
	return make_unique<SDLAudioBackend>();
}