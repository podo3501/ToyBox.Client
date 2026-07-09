#include "pch.h"
#include "AudioService.h"
#include "IAudioBackend.h"
#include "SoundRepository.h"
#include "VoicePool.h"
#include "LoadedSound.h"
#include "PlaybackTypes.h"

struct GroupInfo //지금은 볼륨 하나지만 조금씩 확장될 가능성이 크다.
{
	float volume{ 1.f };
};

struct PendingSoundPlay
{
	VoiceHandle voice;
	SoundHandle sound;
};

unique_ptr<AudioService> AudioService::Create(const SoundAssetView& sndAssetView, unique_ptr<IAudioBackend> backend,
	IAssetAsyncLoader* asyncLoader, int maxVoices, int maxStreams) noexcept
{
	if (backend == nullptr) return nullptr;
	
	unique_ptr<AudioService> service(new AudioService(sndAssetView, move(backend), asyncLoader)); //new를 쓰는 이유는 make_unique를 못 쓰기 때문이다. make_unique는 외부함수이기 때문에 private 생성자에 접근할 수 없다.
	if (!service->Initialize(maxVoices, maxStreams)) return nullptr;

	return service; 
}

AudioService::~AudioService() = default;
AudioService::AudioService(
	const SoundAssetView& sndAssetView, 
	unique_ptr<IAudioBackend> audioBackend,
	IAssetAsyncLoader* asyncLoader) noexcept :
	m_sndAssetView{ make_unique<SoundAssetView>(sndAssetView) },
	m_audioBackend{ move(audioBackend) },
	m_repository{ make_unique<SoundRepository>(m_audioBackend.get(), asyncLoader) },
	m_voicePool{ make_unique<VoicePool>(m_audioBackend.get()) }
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
	for (auto id : EnumUtil::EnumValues<AudioGroup>())
		m_groupInfos[id] = make_unique<GroupInfo>();
}

SoundHandle AudioService::AcquireStaticSound(string_view soundID)
{
	auto staticSoundTable = m_sndAssetView->staticSoundTable;
	if (staticSoundTable == nullptr) return SoundHandle::Invalid();

	auto desc = staticSoundTable->GetDescriptor(soundID);
	if (desc == nullptr) return SoundHandle::Invalid();

	return m_repository->AcquireStaticSound(desc);
}

SoundHandle AudioService::AcquireStreamSound(string_view soundID)
{
	auto streamSoundTable = m_sndAssetView->streamSoundTable;
	if (streamSoundTable == nullptr) return SoundHandle::Invalid();

	auto desc = streamSoundTable->GetDescriptor(soundID);
	if (desc == nullptr) return SoundHandle::Invalid();

	return m_repository->AcquireStreamSound(desc);
}

static void ApplyStaticParams(const StaticSoundDesc* desc, PlaybackParams& params) noexcept
{
	//여기에 새로 추가되는 것들을 params에 추가.
}

static void ApplyStreamParams(const StreamSoundDesc* desc, PlaybackParams& params) noexcept
{
	params.loop = desc->loop;
}

PlaybackParams AudioService::GetParams(const SoundDesc* desc) noexcept
{
	PlaybackParams params;
	params.volume = GetInstanceVolume(desc->group, desc->volume);

	switch (desc->sndType)
	{
	case SoundType::Static: ApplyStaticParams(static_cast<const StaticSoundDesc*>(desc), params); break;
	case SoundType::Stream: ApplyStreamParams(static_cast<const StreamSoundDesc*>(desc), params); break;
	}

	return params;
}

VoiceHandle AudioService::Play(SoundHandle sh) noexcept
{
	auto loaded = m_repository->Find(sh);
	if (!loaded) return VoiceHandle::Invalid();

	if (loaded->state != SoundLoadState::Ready)
	{
		auto desc = loaded->desc;
		if (desc->sndType == SoundType::Stream)
			return EnqueueDeferred(sh, desc);

		return VoiceHandle::Invalid();
	}

	return m_voicePool->Play(sh, loaded, GetParams(loaded->desc));
}

bool AudioService::Pause(VoiceHandle vh) noexcept
{
	return m_voicePool->Pause(vh);
}

bool AudioService::Resume(VoiceHandle vh) noexcept
{
	return m_voicePool->Resume(vh);
}

bool AudioService::Stop(VoiceHandle vh) noexcept
{
	return m_voicePool->StopVoice(vh);
}

bool AudioService::AllStop() noexcept
{
	bool staticOk = m_voicePool->StopVoices(SoundType::Static);
	bool streamOk = m_voicePool->StopVoices(SoundType::Stream);

	return staticOk && streamOk;
}

bool AudioService::Unload(SoundHandle sh) noexcept
{
	auto loaded = m_repository->Find(sh);
	if (loaded == nullptr) return false;

	ReturnIfFalse(m_voicePool->StopVoices(sh));
	return m_repository->Remove(sh);
}

PlaybackState AudioService::GetState(VoiceHandle vh) const noexcept
{
	return m_voicePool->GetState(vh);
}

void AudioService::Update() noexcept
{
	FlushPending();
	m_repository->Update();
	m_voicePool->UpdateVoices();
}

VoiceHandle AudioService::EnqueueDeferred(SoundHandle sh, const SoundDesc* desc)
{
	auto vh = m_voicePool->AcquireVoiceHandle(desc);
	if (!vh) return vh;

	m_pendingPlay.push_back({ vh, sh });
	return vh;
}

void AudioService::FlushPending()
{
	for (auto it = m_pendingPlay.begin(); it != m_pendingPlay.end(); )
	{
		auto& p = *it;
		auto loaded = m_repository->Find(p.sound);
		if (!loaded)
		{
			it = m_pendingPlay.erase(it);
			continue;
		}

		if (loaded->state == SoundLoadState::Pending)
		{
			++it;
			continue;
		}

		if (loaded->state == SoundLoadState::Failed)
		{
			m_voicePool->StopVoice(p.voice);
			it = m_pendingPlay.erase(it);
			continue;
		}

		m_voicePool->Play(p.voice, p.sound, loaded, GetParams(loaded->desc));
		it = m_pendingPlay.erase(it);
	}
}

bool AudioService::SetVolume(VoiceHandle vh, float volume) noexcept
{ 
	auto desc = m_voicePool->GetDesc(vh);
	if (!desc) 
		return false;
	
	auto group = desc->group; 
	if (group == EnumUtil::Invalid<AudioGroup>) 
		return false; 
	
	float curVolume = GetInstanceVolume(group, volume); 
	return m_voicePool->SetVolume(vh, curVolume);
}

float AudioService::GetGroupVolume(AudioGroup group) const noexcept
{
	float groupVolume = m_groupInfos.at(group)->volume;
	float volume = m_masterVolume * groupVolume;

	return std::clamp(volume, 0.f, 1.f);
}

float AudioService::GetInstanceVolume(AudioGroup group, float volume) const noexcept
{
	return GetGroupVolume(group) * volume;
}