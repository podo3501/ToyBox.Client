#include "pch.h"
#include "AudioService.h"
#include "SoundRepository.h"
#include "IAudioBackend.h"
#include "ISoundInstance.h"
#include "Asset/SoundAssetView.h"

struct GroupInfo //지금은 볼륨 하나지만 조금씩 확장될 가능성이 크다.
{
	float volume{ 1.f };
};

struct Voice
{
	ISoundInstance* instance{ nullptr };
	const SoundDescriptor* desc{ nullptr };
	//?!? int soundHandle{ 0 }; 이걸 추가해서 unload할때 이 값으로 비교해서 없애는 것으로 하자. desc로 하지말고.
	//?!? desc 비교를 하지 않게되면 포인터 비교에서 값 비교로 넘어갈 수 있으니까 포인터로 들고 다니지 말고 값으로 넘겨받자.
	
	uint32_t generation{ 0 };

	void Reset() noexcept
	{
		instance = nullptr;
		desc = nullptr;
		//?!? soundHandle = 0;
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
	m_repository{ move(sndRepository)}
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

	return m_repository->AcquireStaticSound(desc);
}

int AudioService::LoadStreamSound(string_view soundID)
{
	auto streamDescriptors = m_sndAssetView->streamDescriptors;
	if (streamDescriptors == nullptr) return 0;

	auto desc = streamDescriptors->GetDescriptor(soundID);
	if (desc == nullptr) return 0;
	
	return m_repository->AcquireStreamSound(desc);
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
	auto loaded = m_repository->Find(soundHandle);
	if (loaded == nullptr) return 0;

	SoundType sndType = loaded->desc->sndType;
	int index = FindFreeVoiceIndex(sndType);
	if (index == -1) return 0;

	auto instance = GetBackendInstance(sndType, loaded->buffer.get());
	if (!instance) return 0;
	
	if (!instance->Reset(GetParams(loaded->desc))) return 0;
	if (!instance->Play()) return 0;

	auto& voice = GetVoiceSlot(sndType, index); //index를 backend로 안보내면서 FindFreeVoiceIndex와 합칠수 있게 되었다.
	voice.instance = instance;
	voice.desc = loaded->desc;
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
	auto loaded = m_repository->Find(soundHandle);
	if (loaded == nullptr) return false;

	auto targetDesc = loaded->desc;
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

	return m_repository->Remove(soundHandle);
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
			auto state = voice.instance->GetState();
			if (state == EnumUtil::Invalid<PlaybackState>)
				int a = 1;
			if (state == PlaybackState::Stopped)
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

ISoundInstance* AudioService::GetBackendInstance(SoundType type, ISoundBuffer* buffer)
{
	switch (type)
	{
	case SoundType::Static: return m_audioBackend->RequestStaticInstance(buffer);
	case SoundType::Stream: return m_audioBackend->RequestStreamInstance(buffer);
	}

	return nullptr;
}