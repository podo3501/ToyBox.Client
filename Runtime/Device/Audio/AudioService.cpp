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

unique_ptr<AudioService> AudioService::Create(SoundTable sndTable, unique_ptr<IAudioBackend> backend,
	IResourceManager* resManager, int maxVoices) noexcept
{
	if (!backend) return nullptr;
	//if (!backend->Initialize(maxVoices)) return nullptr;

	auto service = new AudioService(move(sndTable), move(backend), resManager); //new를 쓰는 이유는 make_unique를 못 쓰기 때문이다. make_unique는 외부함수이기 때문에 private 생성자에 접근할 수 없다.
	if (!service->Initialize(maxVoices)) return nullptr;

	return unique_ptr<AudioService>(service); 
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
	ReturnIfFalse(m_audioBackend->InitializE(maxVoices));

	return true;
}

void AudioService::CreateAudioGroup() noexcept
{
	for (int id : views::iota(0, static_cast<int>(AudioGroupID::None)))
		m_groupInfos[static_cast<AudioGroupID>(id)] = make_unique<GroupInfo>();
}

//int AudioService::LoadSound(string_view soundID)
//{
//	auto info = m_sndTable.GetInfo(soundID);
//	if (!info) return false;
//
//	if (info->sndType == SoundType::Static)
//	{
//		Core::ByteBuffer buffer;
//		ReturnIfFalse(m_resManager->Read(info->filename, buffer));
//
//		int handle = m_audioBackend->LoadStatic(
//			soundID,
//			move(buffer),
//			info->groupID,
//			GetGroupVolume(info->groupID));
//
//		m_playingSounds.insert_or_assign(handle, PlayingSound{ info });
//		return handle;
//	}
//
//	return 0; //잘못된 핸들은 0을 리턴한다.
//}

//int AudioService::LoadSound(string_view soundID)
//{
//	auto info = m_sndTable.GetInfo(soundID);
//	if (!info) return 0;
//
//	if (info->sndType == SoundType::Static)
//	{
//		shared_ptr<ISoundBuffer> sndBuffer;
//		auto it = m_buffers.find(info->filename);
//		if (it != m_buffers.end())
//			sndBuffer = it->second.lock();
//
//		if (!sndBuffer)
//		{
//			Core::ByteBuffer buffer;
//			ReturnIfFalse(m_resManager->Read(info->filename, buffer));
//
//			auto staticBuffer = m_audioBackend->CreateStaticSoundBuffer();
//			if (!staticBuffer) return 0;
//
//			ReturnIfFalse(staticBuffer->LoadFromMemory(move(buffer)));
//			sndBuffer = move(staticBuffer);
//
//			m_buffers.insert_or_assign(info->filename, sndBuffer);
//		}
//
//		int handle = m_audioBackend->CreateInstance(sndBuffer.get(), info->groupID, 
//			GetGroupVolume(info->groupID));
//		if (handle == 0) return 0;
//
//		m_playingSounds.insert_or_assign(handle, PlayingSound{ info });
//		return handle;
//	}
//
//	return 0; //잘못된 핸들은 0을 리턴한다.
//}

int AudioService::LoadSound(string_view soundID)
{
	auto info = m_sndTable.GetInfo(soundID);
	if (!info) return 0;

	if (info->sndType != SoundType::Static)
		return 0;

	shared_ptr<ISoundBuffer> sndBuffer;
	auto it = m_buffers.find(info->filename);
	if (it != m_buffers.end())
		sndBuffer = it->second.lock();

	if (!sndBuffer)
	{
		Core::ByteBuffer buffer;
		ReturnIfFalse(m_resManager->Read(info->filename, buffer));

		auto staticBuffer = m_audioBackend->CreateStaticSoundBuffer();
		if (!staticBuffer) return 0;

		ReturnIfFalse(staticBuffer->LoadFromMemory(move(buffer)));

		sndBuffer = move(staticBuffer);
		m_buffers.insert_or_assign(info->filename, sndBuffer);
	}
	int soundHandle = m_nextSoundHandle++;
	m_loadedSounds.emplace(soundHandle, LoadedSound{ info, sndBuffer });

	return soundHandle;
}

//int AudioService::Play(int soundHandle) noexcept
//{
//	auto it = m_loadedSounds.find(soundHandle);
//	if (it == m_loadedSounds.end()) return 0;
//
//	auto& loaded = it->second;
//	int instanceHandle = m_audioBackend->CreateInstance(
//		loaded.buffer.get(),
//		loaded.info->groupID,
//		GetGroupVolume(loaded.info->groupID));
//	if (instanceHandle == 0) return 0;
//
//	m_playingSounds.insert_or_assign(instanceHandle, PlayingSound{ loaded.info });
//	if(!m_audioBackend->Play(instanceHandle)) return 0;
//
//	return instanceHandle;
//}

int AudioService::FindFreeVoiceIndex() noexcept
{
	for (size_t i = 0; i < m_voices.size(); ++i)
	{
		int index = m_cycleIter.Increase();
		auto& voice = m_voices[index];
		if (voice.active) continue;

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
	voice.active = true;
	voice.generation++;
	voice.instance = instance;
	
	return MakeHandle(index, voice.generation);
}

void AudioService::Stop(int instanceHandle) noexcept
{
	auto it = m_playingSounds.find(instanceHandle);
	if (it == m_playingSounds.end()) return;

	m_audioBackend->Stop(instanceHandle);
	m_playingSounds.erase(it);
}

bool AudioService::Unload(int soundHandle) noexcept
{
	auto it = m_loadedSounds.find(soundHandle);
	if (it == m_loadedSounds.end()) return false;

	const SoundInfo* target = it->second.info;
	for (auto& [instHandle, playing] : m_playingSounds)
	{
		if (playing.info == target)
			Stop(instHandle);
	}

	m_loadedSounds.erase(it);
	return true;
}

bool AudioService::Load(string_view soundID)
{
	auto info = m_sndTable.GetInfo(soundID);
	if (!info) return false;

	if (info->sndType == SoundType::Static)
	{
		return false;
		//Core::ByteBuffer buffer;
		//ReturnIfFalse(m_resManager->Read(info->filename, buffer));

		//return m_audioBackend->LoadStatic(
		//	soundID,
		//	move(buffer),
		//	info->groupID,
		//	GetGroupVolume(info->groupID));
	}
	
	if (info->sndType == SoundType::Stream)
	{
		auto stream = m_resManager->CreateReadStream(info->filename);
		if (!stream) return false;

		return m_audioBackend->LoadStream(
			soundID,
			move(stream),
			info->groupID,
			GetGroupVolume(info->groupID),
			false);
	}

	return false;
}

bool AudioService::Unload(string_view soundID) noexcept
{
	return m_audioBackend->Unload(soundID);
}

//bool AudioService::Unload(int handle) noexcept
//{
//	return m_audioBackend->Unload(handle);
//}

bool AudioService::Play(string_view soundID) noexcept
{
	return m_audioBackend->Play(soundID);
}

//bool AudioService::Play(int handle) noexcept
//{
//	return m_audioBackend->Play(handle);
//}

PlayState AudioService::GetState(string_view soundID) const noexcept
{
	return m_audioBackend->GetState(soundID);
}

PlayState AudioService::GetState(int handle) const noexcept
{
	auto instance = GetInstance(handle);
	if (instance == nullptr) return PlayState::None;

	return instance->IsPlaying() ? PlayState::Playing : PlayState::None;
}

void AudioService::Update() noexcept
{
	m_audioBackend->Update();
}

bool AudioService::SetVolume(string_view soundID, float volume) noexcept
{
	auto groupID = GetGroupID(soundID);
	if (groupID == AudioGroupID::None) return false;

	float curVolume = GetGroupVolume(groupID) * volume;
	return m_audioBackend->SetVolume(soundID, curVolume);
}

bool AudioService::SetVolumE(int handle, float volume) noexcept
{
	auto it = m_playingSounds.find(handle);
	if (it == m_playingSounds.end()) return false;

	auto& ps = it->second;
	auto groupID = ps.info->groupID;
	if (groupID == AudioGroupID::None) return false;
	if (ps.info->volume != volume) ps.volume = volume;

	float curVolume = GetGroupVolume(groupID) * ps.volume;
	return m_audioBackend->SetVolume(handle, curVolume);
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
	if (!info) return AudioGroupID::None;

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
	if (!voice.active || voice.generation != gen) // 슬롯이 비활성 상태거나 세대가 다르면 nullptr 반환
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