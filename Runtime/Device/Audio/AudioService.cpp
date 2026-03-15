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

struct LoadedSound
{
	const SoundInfo* info;
	shared_ptr<ISoundBuffer> buffer;
};

struct Voice
{
	ISoundInstance* instance{ nullptr };
	const SoundInfo* info{ nullptr };
	uint32_t generation{ 0 };

	bool Stop() noexcept
	{
		if (!instance) return false;
		return instance->Stop();
	}

	void Reset() noexcept
	{
		instance = nullptr;
		info = nullptr;
	}

	bool StopAndReset() noexcept
	{
		ReturnIfFalse(Stop());
		Reset();

		return true;
	}
};

unique_ptr<AudioService> AudioService::Create(StaticSoundTable staticTable, StreamSoundTable streamTable, 
	unique_ptr<IAudioBackend> backend, IResourceManager* resManager, int maxVoices, int maxStreams) noexcept
{
	if (!backend) return nullptr;

	unique_ptr<AudioService> service(
		new AudioService(move(staticTable), move(streamTable), move(backend), resManager)); //new를 쓰는 이유는 make_unique를 못 쓰기 때문이다. make_unique는 외부함수이기 때문에 private 생성자에 접근할 수 없다.
	if (!service->Initialize(maxVoices, maxStreams)) return nullptr;

	return service; 
}

AudioService::~AudioService() = default;
AudioService::AudioService(StaticSoundTable staticTable, StreamSoundTable streamTable,
	unique_ptr<IAudioBackend> audioBackend, IResourceManager* resManager) noexcept
	: m_staticTable{ move(staticTable) },
	m_streamTable{ move(streamTable) },
	m_audioBackend{ move(audioBackend) },
	m_resManager{ resManager }
{}

bool AudioService::Initialize(int maxVoices, int maxStreams) noexcept
{
	CreateAudioGroup();
	if (maxVoices > 0) m_staticCycleIter.SetRange(0, maxVoices); //인덱스를 차례로 사용하게 하는 순환인덱스 생성.
	if (maxStreams > 0) m_streamCycleIter.SetRange(0, maxStreams);

	m_staticVoices.resize(maxVoices);
	m_streamVoices.resize(maxStreams);
	ReturnIfFalse(m_audioBackend->Initialize(maxVoices, maxStreams));

	return true;
}

void AudioService::CreateAudioGroup() noexcept
{
	for (auto id : EnumUtil::EnumValues<AudioGroupID>())
		m_groupInfos[id] = make_unique<GroupInfo>();
}

int AudioService::LoadStaticSound(string_view soundID)
{
	auto info = m_staticTable.GetInfo(soundID);
	if (info == nullptr) return 0;

	return LoadSoundInternal(info,
		[this](const StaticSoundInfo* i) { return CreateStaticSoundBuffer(i); });
}

int AudioService::LoadStreamSound(string_view soundID)
{
	auto info = m_streamTable.GetInfo(soundID);
	if (info == nullptr) return 0;

	return LoadSoundInternal(info,
		[this](const StreamSoundInfo* i) { return CreateStreamSoundBuffer(i); });
}

template<typename InfoType, typename CreateFunc>
int AudioService::LoadSoundInternal(const InfoType* info, CreateFunc createFunc)
{
	shared_ptr<ISoundBuffer> sndBuffer;

	auto it = m_buffers.find(info->filename);
	if (it != m_buffers.end())
		sndBuffer = it->second.lock();

	if (!sndBuffer)
	{
		sndBuffer = createFunc(info);
		if (!sndBuffer) return 0;

		m_buffers.insert_or_assign(info->filename, sndBuffer);
	}

	int soundHandle = m_nextSoundHandle++;
	m_loadedSounds.emplace(soundHandle, LoadedSound{ info, sndBuffer });

	return soundHandle;
}

shared_ptr<ISoundBuffer> AudioService::CreateStaticSoundBuffer(const StaticSoundInfo* info)
{
	Core::ByteBuffer buffer;
	if (!m_resManager->Read(info->filename, buffer)) return nullptr;

	auto staticBuffer = m_audioBackend->CreateStaticSoundBuffer();
	if (!staticBuffer) return nullptr;

	if (!staticBuffer->LoadFromMemory(move(buffer))) return nullptr;
	return staticBuffer;
}

shared_ptr<ISoundBuffer> AudioService::CreateStreamSoundBuffer(const StreamSoundInfo* info)
{
	auto streamBuffer = m_audioBackend->CreateStreamSoundBuffer();
	if (!streamBuffer) return nullptr;

	auto stream = m_resManager->CreateReadStream(info->filename);
	if (!stream) return nullptr;

	if (!streamBuffer->AttachStream(move(stream))) return nullptr;
	return streamBuffer;
}

int AudioService::FindFreeVoiceIndex(SoundType type) noexcept
{
	auto& voices = (type == SoundType::Static) ? m_staticVoices : m_streamVoices;
	auto& iter = (type == SoundType::Static) ? m_staticCycleIter : m_streamCycleIter;
	if (voices.empty()) return -1;

	for (size_t i = 0; i < voices.size(); ++i)
	{
		int index = iter.Increase();
		auto& voice = voices[index];
		if (voice.instance != nullptr) continue;

		return index;
	}

	return -1;
}

static int MakeHandle(uint16_t index, uint16_t generation)
{
	return (index & 0xFFFF) | (generation << 16); // 32비트 핸들 생성: 하위 16비트 = 슬롯 인덱스, 상위 16비트 = 세대;
}

PlaybackParams AudioService::GetParams(const SoundInfo* info)
{
	PlaybackParams params;
	params.volume = GetInstanceVolume(info->groupID, info->volume);
	
	if (info->sndType == SoundType::Static)
	{
		const StaticSoundInfo* staticInfo = static_cast<const StaticSoundInfo*>(info);
		//여기에 새로 추가되는 것들을 params에 추가.
	}

	if (info->sndType == SoundType::Stream)
	{
		const StreamSoundInfo* streamInfo = static_cast<const StreamSoundInfo*>(info);
		params.loop = streamInfo->loop;
	}

	return params;
}

int AudioService::Play(int soundHandle) noexcept
{
	auto it = m_loadedSounds.find(soundHandle);
	if (it == m_loadedSounds.end()) return 0;

	auto& loaded = it->second;
	SoundType sndType = loaded.info->sndType;
	int index = FindFreeVoiceIndex(sndType);
	if (index == -1) return 0;

	auto instance = m_audioBackend->AcquireInstance(sndType, loaded.buffer.get(), index);
	if (!instance) return 0;
	
	if (!instance->Reset(GetParams(loaded.info))) return 0;
	if (!instance->Play()) return 0;

	auto& voice = GetVoiceSlot(sndType, index);
	voice.instance = instance;
	voice.info = loaded.info;
	voice.generation++;
	
	int baseIndex = (sndType == SoundType::Stream) ? static_cast<int>(m_staticVoices.size()) : 0;
	return MakeHandle(baseIndex + index, voice.generation);
}

bool AudioService::Pause(int instanceHandle) noexcept
{
	auto instance = GetInstance(instanceHandle);
	if (instance == nullptr) return false;

	return instance->Pause();
}

bool AudioService::Resume(int instanceHandle) noexcept
{
	auto instance = GetInstance(instanceHandle);
	if (instance == nullptr) return false;

	return instance->Resume();
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
	auto stopVoices = [targetInfo](auto& voices) {
		for (auto& voice : voices)
		{
			if (voice.instance == nullptr) continue;
			if (voice.info != targetInfo) continue;

			ReturnIfFalse(voice.StopAndReset());
		}
		return true;	};

	ReturnIfFalse(stopVoices(m_staticVoices));
	ReturnIfFalse(stopVoices(m_streamVoices));

	m_loadedSounds.erase(it);
	return true;
}

PlaybackState AudioService::GetState(int handle) const noexcept
{
	auto instance = GetInstance(handle);
	if (instance == nullptr) return EnumUtil::Invalid<PlaybackState>;

	return instance->GetState();
}

void AudioService::Update() noexcept
{
	auto updateVoices = [](auto& voices) {
		for (auto& voice : voices)
		{
			if (!voice.instance) continue;

			voice.instance->Update();
			if (voice.instance->GetState() == PlaybackState::Stopped)
				voice.Reset();
		}};

	updateVoices(m_staticVoices);
	updateVoices(m_streamVoices);
}

bool AudioService::SetVolume(int instanceHandle, float volume) noexcept
{
	auto voice = GetVoice(instanceHandle);
	if (voice == nullptr) return false;

	auto& info = *voice->info;
	auto& instance = *voice->instance;

	auto groupID = info.groupID;
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

Voice& AudioService::GetVoiceSlot(SoundType type, int index) noexcept
{
	assert(type == SoundType::Static || type == SoundType::Stream);
	
	if (type == SoundType::Static) 
		return m_staticVoices[index];
	return m_streamVoices[index];
}

const Voice* AudioService::GetVoice(int handle) const noexcept
{
	if (handle <= 0) return nullptr;

	// 핸들에서 슬롯 인덱스와 세대 추출
	size_t index = static_cast<size_t>(handle & 0xFFFF);
	uint16_t gen = static_cast<uint16_t>((handle >> 16) & 0xFFFF);
	if (index >= m_staticVoices.size() + m_streamVoices.size()) return nullptr;

	const Voice* voice = (index < m_staticVoices.size())
		? &m_staticVoices[index]
		: &m_streamVoices[index - m_staticVoices.size()];

	if (voice->instance == nullptr || voice->generation != gen) // instance가 없거나 세대가 다르면 nullptr 반환
		return nullptr;

	return voice;
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
