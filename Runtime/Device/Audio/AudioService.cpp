#include "pch.h"
#include "AudioService.h"
#include "IAudioBackend.h"
#include "ISoundBuffer.h"
#include "ISoundInstance.h"
#include "Platform/Resource/IResourceManager.h"

struct GroupInfo //지금은 볼륨 하나지만 조금씩 확장될 가능성이 크다.
{
	float volume{ 1.f };
};

struct PlayingSound //사운드 테이블값과 인스턴스별 속성값.
{
	PlayingSound(const SoundInfo* _info) : info{ _info }, volume{ _info->volume }
	{}

	const SoundInfo* info{ nullptr };
	float volume{ 1.0f };
};

struct Voice
{
	ISoundInstance* instance{ nullptr };
	const SoundInfo* info{ nullptr };
	uint32_t generation{ 0 };

	bool StopAndReset() noexcept
	{
		if (!instance) return false;
		if (!instance->Stop()) return false;

		instance = nullptr;
		info = nullptr;
		return true;
	}
};

unique_ptr<AudioService> AudioService::Create(SoundTable sndTable, unique_ptr<IAudioBackend> backend,
	IResourceManager* resManager, int maxVoices) noexcept
{
	if (!backend) return nullptr;
	//if (!backend->Initialize(maxVoices)) return nullptr;

	unique_ptr<AudioService> service(
		new AudioService(move(sndTable), move(backend), resManager)); //new를 쓰는 이유는 make_unique를 못 쓰기 때문이다. make_unique는 외부함수이기 때문에 private 생성자에 접근할 수 없다.
	if (!service->Initialize(maxVoices)) return nullptr;

	return service; 
}

AudioService::~AudioService() = default;
AudioService::AudioService(SoundTable sndTable, unique_ptr<IAudioBackend> audioBackend, 
	IResourceManager* resManager) noexcept
	: m_sndTable{ move(sndTable) },
	m_audioBackend{ move(audioBackend) },
	m_resManager{ resManager }
{}

bool AudioService::Initialize(int maxVoices) noexcept
{
	CreateAudioGroup();
	m_cycleIter.SetRange(0, maxVoices); //인덱스를 차례로 사용하게 하는 순환인덱스 생성.
	m_voices.resize(maxVoices);
	ReturnIfFalse(m_audioBackend->Initialize(maxVoices));

	return true;
}

void AudioService::CreateAudioGroup() noexcept
{
	for (auto id : EnumUtil::EnumValues<AudioGroupID>())
		m_groupInfos[id] = make_unique<GroupInfo>();
}

int AudioService::LoadSound(string_view soundID)
{
	auto info = m_sndTable.GetInfo(soundID);
	if (!info) return 0;

	shared_ptr<ISoundBuffer> sndBuffer;
	auto it = m_buffers.find(info->filename);
	if (it != m_buffers.end())
		sndBuffer = it->second.lock();

	if (!sndBuffer)
	{
		sndBuffer = CreateSoundBuffer(info);
		if (sndBuffer == nullptr) return 0;

		m_buffers.insert_or_assign(info->filename, sndBuffer);
	}

	int soundHandle = m_nextSoundHandle++;
	m_loadedSounds.emplace(soundHandle, LoadedSound{ info, sndBuffer });

	return soundHandle;
}

shared_ptr<ISoundBuffer> AudioService::CreateSoundBuffer(const SoundInfo* info)
{
	if(info->sndType == SoundType::Static)
	{
		Core::ByteBuffer buffer;
		if (!m_resManager->Read(info->filename, buffer)) return nullptr;

		auto staticBuffer = m_audioBackend->CreateStaticSoundBuffer();
		if (!staticBuffer) return nullptr;

		if (!staticBuffer->LoadFromMemory(move(buffer))) return nullptr;
		return staticBuffer;
	}

	if (info->sndType == SoundType::Stream)
	{
		auto streamBuffer = m_audioBackend->CreateStreamSoundBuffer();
		if (!streamBuffer) return nullptr;

		auto stream = m_resManager->CreateReadStream(info->filename);
		if (!stream) return nullptr;

		if (!streamBuffer->AttachStream(move(stream))) return nullptr;
		return streamBuffer;
	}

	return nullptr;
}

int AudioService::FindFreeVoiceIndex() noexcept
{
	for (size_t i = 0; i < m_voices.size(); ++i)
	{
		int index = m_cycleIter.Increase();
		auto& voice = m_voices[index];
		if (voice.instance != nullptr) continue;

		return index;
	}

	return -1;
}

static int MakeHandle(uint16_t index, uint16_t generation)
{
	return (index & 0xFFFF) | (generation << 16); // 32비트 핸들 생성: 하위 16비트 = 슬롯 인덱스, 상위 16비트 = 세대;
}

int AudioService::Play(int soundHandle) noexcept
{
	auto it = m_loadedSounds.find(soundHandle);
	if (it == m_loadedSounds.end()) return 0;
	
	int index = FindFreeVoiceIndex();
	if (index == -1) return 0;

	auto& loaded = it->second;
	auto instance = m_audioBackend->AcquireInstance(loaded.buffer.get(), index);
	if (!instance) return 0;

	if (!instance->Reset(GetGroupVolume(loaded.info->groupID))) return 0;
	if (!instance->Play()) return 0;

	auto& voice = m_voices[index];
	voice.instance = instance;
	voice.info = loaded.info;
	voice.generation++;
	
	return MakeHandle(index, voice.generation);
}

bool AudioService::Pause(int instanceHandle) noexcept
{
	auto instance = GetInstance(instanceHandle);
	if (instance == nullptr) return false;

	return instance->Pause();
}

bool AudioService::Stop(int instanceHandle) noexcept
{
	auto voice = GetVoice(instanceHandle);
	if (voice == nullptr) return false;

	return voice->StopAndReset();
}

bool AudioService::Unload(int soundHandle) noexcept
{
	auto it = m_loadedSounds.find(soundHandle);
	if (it == m_loadedSounds.end()) return false;

	auto targetInfo = it->second.info; 
	for (auto& voice : m_voices)
	{
		if (voice.instance == nullptr) continue;
		if (voice.info != targetInfo) continue;

		ReturnIfFalse(voice.StopAndReset());
	}

	m_loadedSounds.erase(it);
	return true;
}

PlayState AudioService::GetState(int handle) const noexcept
{
	auto instance = GetInstance(handle);
	if (instance == nullptr) return EnumUtil::Invalid<PlayState>;

	return instance->IsPlaying() ? PlayState::Playing : EnumUtil::Invalid<PlayState>;
}

void AudioService::Update() noexcept
{
	m_audioBackend->Update();
}

bool AudioService::SetVolume(int instanceHandle, float volume) noexcept
{
	auto voice = GetVoice(instanceHandle);
	if (voice == nullptr) return false;

	auto& info = *voice->info;
	auto& instance = *voice->instance;

	auto groupID = info.groupID;
	if (groupID == EnumUtil::Invalid<AudioGroupID>) return false;

	float curVolume = GetGroupVolume(groupID) * volume;
	return instance.SetVolume(curVolume);
}

float AudioService::GetGroupVolume(AudioGroupID groupID) const noexcept
{
	float groupVolume = m_groupInfos.at(groupID)->volume;
	float volume = m_masterVolume * groupVolume;

	return std::clamp(volume, 0.f, 1.f);
}

AudioGroupID AudioService::GetGroupID(string_view soundID)
{
	auto info = m_sndTable.GetInfo(soundID);
	if (!info) return EnumUtil::Invalid<AudioGroupID>;

	return info->groupID;
}

const Voice* AudioService::GetVoice(int handle) const noexcept
{
	if (handle <= 0) return nullptr;

	// 핸들에서 슬롯 인덱스와 세대 추출
	size_t index = static_cast<size_t>(handle & 0xFFFF);
	uint16_t gen = static_cast<uint16_t>((handle >> 16) & 0xFFFF);
	if (index >= m_voices.size()) return nullptr;

	const auto& voice = m_voices[index];
	if (voice.instance == nullptr || voice.generation != gen) // instance가 없거나 세대가 다르면 nullptr 반환
		return nullptr;

	return &voice;
}

Voice* AudioService::GetVoice(int handle) noexcept
{
	return const_cast<Voice*>(static_cast<const AudioService*>(this)->GetVoice(handle));
}

const ISoundInstance* AudioService::GetInstance(int handle) const noexcept
{
	auto voice = GetVoice(handle);
	if (voice == nullptr) return nullptr;

	return voice->instance;
}

ISoundInstance* AudioService::GetInstance(int handle) noexcept
{
	return const_cast<ISoundInstance*>(static_cast<const AudioService*>(this)->GetInstance(handle));
}