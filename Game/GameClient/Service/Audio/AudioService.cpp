#include "pch.h"
#include "AudioService.h"
#include "SoundRepository.h"
#include "VoicePool.h"
#include "IAudioBackend.h"
#include "ISoundInstance.h"
#include "Asset/SoundAssetView.h"

struct GroupInfo //지금은 볼륨 하나지만 조금씩 확장될 가능성이 크다.
{
	float volume{ 1.f };
};

unique_ptr<AudioService> AudioService::Create(const SoundAssetView* sndAssetView, unique_ptr<IAudioBackend> backend,
	IResourceManager* resManager, int maxVoices, int maxStreams) noexcept
{
	if (sndAssetView == nullptr) return nullptr;
	if (backend == nullptr) return nullptr;
	
	auto sndRepository = make_unique<SoundRepository>(backend.get(), resManager);

	unique_ptr<AudioService> service(
		new AudioService(sndAssetView, move(sndRepository), move(backend))); //new를 쓰는 이유는 make_unique를 못 쓰기 때문이다. make_unique는 외부함수이기 때문에 private 생성자에 접근할 수 없다.
	if (!service->Initialize(maxVoices, maxStreams)) return nullptr;

	return service; 
}

AudioService::~AudioService() = default;
AudioService::AudioService(const SoundAssetView* sndAssetView, unique_ptr<SoundRepository> sndRepository,
	unique_ptr<IAudioBackend> audioBackend) noexcept :
	m_sndAssetView{ sndAssetView },
	m_audioBackend{ move(audioBackend) },
	m_repository{ move(sndRepository)},
	m_voicePool{ make_unique<VoicePool>() }
{}

bool AudioService::Initialize(int maxVoices, int maxStreams) noexcept
{
	ReturnIfFalse(m_audioBackend->Initialize(maxVoices, maxStreams));
	ReturnIfFalse(m_voicePool->Setup(maxVoices, maxStreams));
	CreateAudioGroup();

	return true;
}

void AudioService::CreateAudioGroup() noexcept
{
	for (auto id : EnumUtil::EnumValues<AudioGroupID>())
		m_groupInfos[id] = make_unique<GroupInfo>();
}

SoundHandle AudioService::LoadStaticSound(string_view soundID)
{
	auto staticDescriptors = m_sndAssetView->staticDescriptors;
	if (staticDescriptors == nullptr) return InvalidSoundHandle;

	auto desc = staticDescriptors->GetDescriptor(soundID);
	if (desc == nullptr) return InvalidSoundHandle;

	return m_repository->AcquireStaticSound(desc);
}

SoundHandle AudioService::LoadStreamSound(string_view soundID)
{
	auto streamDescriptors = m_sndAssetView->streamDescriptors;
	if (streamDescriptors == nullptr) return InvalidSoundHandle;

	auto desc = streamDescriptors->GetDescriptor(soundID);
	if (desc == nullptr) return InvalidSoundHandle;
	
	return m_repository->AcquireStreamSound(desc);
}

PlaybackParams AudioService::GetParams(const SoundDescriptor* desc)
{
	PlaybackParams params;
	params.volume = GetInstanceVolume(desc->groupID, desc->volume);
	
	if (desc->sndType == SoundType::Static)
	{
		const StaticSoundDescriptor* staticDesc = static_cast<const StaticSoundDescriptor*>(desc);
		//여기에 새로 추가되는 것들을 params에 추가.
	}

	if (desc->sndType == SoundType::Stream)
	{
		const StreamSoundDescriptor* streamDesc = static_cast<const StreamSoundDescriptor*>(desc);
		params.loop = streamDesc->loop;
	}

	return params;
}

int AudioService::Play(SoundHandle h) noexcept
{
	auto loaded = m_repository->Find(h);
	if (loaded == nullptr) return 0;

	auto desc = loaded->desc;
	SoundType sndType = desc->sndType;
	auto instance = GetBackendInstance(sndType, loaded->buffer.get());
	if (!instance) return 0;
	
	if (!instance->Reset(GetParams(desc))) return 0;
	if (!instance->Play()) return 0;

	return m_voicePool->AcquireVoice(sndType, loaded, instance);
}

bool AudioService::Pause(int instanceHandle) noexcept
{
	auto instance = m_voicePool->GetInstance(instanceHandle);
	if (instance == nullptr) return false;

	return instance->Pause();
}

bool AudioService::Resume(int instanceHandle) noexcept
{
	auto instance = m_voicePool->GetInstance(instanceHandle);
	if (instance == nullptr) return false;

	return instance->Resume();
}

bool AudioService::Stop(int instanceHandle) noexcept
{
	auto voice = m_voicePool->GetVoice(instanceHandle);
	if (voice == nullptr) return false;

	return voice->StopAndReset();
}

bool AudioService::AllStop() noexcept
{
	bool staticOk = m_voicePool->StopAllStaticVoices();
	bool streamOk = m_voicePool->StopAllStreamVoices();

	return staticOk && streamOk;
}

bool AudioService::Unload(SoundHandle h) noexcept
{
	auto loaded = m_repository->Find(h);
	if (loaded == nullptr) return false;

	ReturnIfFalse(m_voicePool->StopStaticVoices(loaded));
	ReturnIfFalse(m_voicePool->StopStreamVoices(loaded));

	return m_repository->Remove(h);
}

PlaybackState AudioService::GetState(int handle) const noexcept
{
	auto instance = m_voicePool->GetInstance(handle);
	if (instance == nullptr) return EnumUtil::Invalid<PlaybackState>;

	return instance->GetState();
}

void AudioService::Update() noexcept
{
	m_voicePool->UpdateStaticVoices();
	m_voicePool->UpdateStreamVoices();
}

bool AudioService::SetVolume(int instanceHandle, float volume) noexcept
{
	auto voice = m_voicePool->GetVoice(instanceHandle);
	if (voice == nullptr) return false;

	auto& desc = *voice->loaded->desc;
	auto& instance = *voice->instance;

	auto groupID = desc.groupID;
	if (groupID == EnumUtil::Invalid<AudioGroupID>) return false;

	return instance.SetVolume(GetInstanceVolume(groupID, volume));
}

float AudioService::GetGroupVolume(AudioGroupID groupID) const noexcept
{
	float groupVolume = m_groupInfos.at(groupID)->volume;
	float volume = m_masterVolume * groupVolume;

	return std::clamp(volume, 0.f, 1.f);
}

float AudioService::GetInstanceVolume(AudioGroupID groupID, float volume) const noexcept
{
	return GetGroupVolume(groupID) * volume;
}

ISoundInstance* AudioService::GetBackendInstance(SoundType type, ISoundBuffer* buffer)
{
	switch (type)
	{
	case SoundType::Static: return m_audioBackend->RequestStaticInstance(buffer);
	case SoundType::Stream: return m_audioBackend->RequestStreamInstance(buffer);
	}

	return nullptr;
}