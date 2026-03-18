#include "pch.h"
#include "AudioService.h"
#include "IAudioBackend.h"
#include "ISoundBuffer.h"
#include "ISoundInstance.h"
#include "Asset/SoundAssetView.h"
#include "Platform/Resource/IResourceManager.h"

struct GroupInfo //지금은 볼륨 하나지만 조금씩 확장될 가능성이 크다.
{
	float volume{ 1.f };
};

struct LoadedSound
{
	const SoundDescriptor* desc;
	shared_ptr<ISoundBuffer> buffer;
};

struct Voice
{
	ISoundInstance* instance{ nullptr };
	const SoundDescriptor* desc{ nullptr };
	uint32_t generation{ 0 };

	void Reset() noexcept
	{
		instance = nullptr;
		desc = nullptr;
	}

	bool StopAndReset() noexcept
	{
		if (!instance) return true;

		ReturnIfFalse(instance->Stop());
		Reset();

		return true;
	}
};

unique_ptr<AudioService> AudioService::Create(const SoundAssetView* sndAssetView, unique_ptr<IAudioBackend> backend,
	IResourceManager* resManager, int maxVoices, int maxStreams) noexcept
{
	if (sndAssetView == nullptr) return nullptr;
	if (backend == nullptr) return nullptr;

	unique_ptr<AudioService> service(
		new AudioService(sndAssetView, move(backend), resManager)); //new를 쓰는 이유는 make_unique를 못 쓰기 때문이다. make_unique는 외부함수이기 때문에 private 생성자에 접근할 수 없다.
	if (!service->Initialize(maxVoices, maxStreams)) return nullptr;

	return service; 
}

AudioService::~AudioService() = default;
AudioService::AudioService(const SoundAssetView* sndAssetView,
	unique_ptr<IAudioBackend> audioBackend, IResourceManager* resManager) noexcept :
	m_sndAssetView{ sndAssetView },
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
	auto staticDescriptors = m_sndAssetView->staticDescriptors;
	if (staticDescriptors == nullptr) return 0;

	auto desc = staticDescriptors->GetDescriptor(soundID);
	if (desc == nullptr) return 0;

	return LoadSoundInternal(desc,
		[this](const StaticSoundDescriptor* i) { return CreateStaticSoundBuffer(i); });
}

int AudioService::LoadStreamSound(string_view soundID)
{
	auto streamDescriptors = m_sndAssetView->streamDescriptors;
	if (streamDescriptors == nullptr) return 0;

	auto desc = streamDescriptors->GetDescriptor(soundID);
	if (desc == nullptr) return 0;

	return LoadSoundInternal(desc,
		[this](const StreamSoundDescriptor* i) { return CreateStreamSoundBuffer(i); });
}

template<typename DescType, typename CreateFunc>
int AudioService::LoadSoundInternal(const DescType* desc, CreateFunc createFunc)
{
	shared_ptr<ISoundBuffer> sndBuffer;

	auto it = m_buffers.find(desc->filename);
	if (it != m_buffers.end())
		sndBuffer = it->second.lock();

	if (!sndBuffer)
	{
		sndBuffer = createFunc(desc);
		if (!sndBuffer) return 0;

		m_buffers.insert_or_assign(desc->filename, sndBuffer);
	}

	int soundHandle = m_nextSoundHandle++;
	m_loadedSounds.emplace(soundHandle, LoadedSound{ desc, sndBuffer });

	return soundHandle;
}

shared_ptr<ISoundBuffer> AudioService::CreateStaticSoundBuffer(const StaticSoundDescriptor* desc)
{
	Core::ByteBuffer buffer;
	if (!m_resManager->Read(desc->filename, buffer)) return nullptr;

	auto staticBuffer = m_audioBackend->CreateStaticSoundBuffer();
	if (!staticBuffer) return nullptr;

	if (!staticBuffer->LoadFromMemory(move(buffer))) return nullptr;
	return staticBuffer;
}

shared_ptr<ISoundBuffer> AudioService::CreateStreamSoundBuffer(const StreamSoundDescriptor* desc)
{
	auto streamBuffer = m_audioBackend->CreateStreamSoundBuffer();
	if (!streamBuffer) return nullptr;

	auto stream = m_resManager->CreateReadStream(desc->filename);
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

int AudioService::Play(int soundHandle) noexcept
{
	auto it = m_loadedSounds.find(soundHandle);
	if (it == m_loadedSounds.end()) return 0;

	auto& loaded = it->second;
	SoundType sndType = loaded.desc->sndType;
	int index = FindFreeVoiceIndex(sndType);
	if (index == -1) return 0;

	auto instance = GetBackendInstance(sndType, loaded.buffer.get(), index);
	if (!instance) return 0;
	
	if (!instance->Reset(GetParams(loaded.desc))) return 0;
	if (!instance->Play()) return 0;

	auto& voice = GetVoiceSlot(sndType, index);
	voice.instance = instance;
	voice.desc = loaded.desc;
	voice.generation = (voice.generation + 1) & 0xFFFF;
	
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

bool AudioService::AllStop() noexcept
{
	bool staticOk = ranges::all_of(m_staticVoices, [](auto& voice) { return voice.StopAndReset(); });
	bool streamOk = ranges::all_of(m_streamVoices, [](auto& voice) { return voice.StopAndReset(); });

	return staticOk && streamOk;
}

bool AudioService::Unload(int soundHandle) noexcept
{
	auto it = m_loadedSounds.find(soundHandle);
	if (it == m_loadedSounds.end()) return false;

	auto targetDesc = it->second.desc; 
	auto stopVoices = [targetDesc](auto& voices) {
		for (auto& voice : voices)
		{
			if (voice.instance == nullptr) continue;
			if (voice.desc != targetDesc) continue;

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

	auto& desc = *voice->desc;
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

ISoundInstance* AudioService::GetBackendInstance(SoundType type, ISoundBuffer* buffer, int index)
{
	switch (type)
	{
	case SoundType::Static: return m_audioBackend->RequestStaticInstance(buffer, index);
	case SoundType::Stream: return m_audioBackend->RequestStreamInstance(buffer, index);
	}

	return nullptr;
}