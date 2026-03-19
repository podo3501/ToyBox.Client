#pragma once
#include "Core/Utils/CycleIterator.h"

struct IAudioBackend;
struct IResourceManager;
struct SoundAssetView;
struct ISoundBuffer;
struct ISoundInstance;
struct GroupInfo;
struct Voice;
struct PlaybackParams;
struct SoundDescriptor;
class SoundRepository;
enum class PlaybackState;
enum class AudioGroupID;
enum class SoundType;

class AudioService
{
public:
	~AudioService();
	AudioService() = delete;
	static unique_ptr<AudioService> Create(const SoundAssetView* sndAssetView, unique_ptr<IAudioBackend> backend,
		IResourceManager* resManager, int maxVoices, int maxStreams) noexcept;
	int LoadStaticSound(string_view soundID);
	int LoadStreamSound(string_view soundID);
	bool Unload(int soundHandle) noexcept;
	int Play(int soundHandle) noexcept;
	bool Pause(int instanceHandle) noexcept;
	bool Resume(int instanceHandle) noexcept;
	bool Stop(int instanceHandle) noexcept;
	bool AllStop() noexcept;
	void Update() noexcept;
	PlaybackState GetState(int handle) const noexcept;
	inline void SetMasterVolume(float volume) noexcept { m_masterVolume = volume; }
	inline float GetMasterVolume() const noexcept { return m_masterVolume; }
	bool SetVolume(int handle, float volume) noexcept;

private:
	AudioService(const SoundAssetView* sndAssetView, unique_ptr<SoundRepository> sndRepository,
		unique_ptr<IAudioBackend> audioBackend) noexcept;
	bool Initialize(int maxVoices, int maxStreams) noexcept;
	void CreateAudioGroup() noexcept;
	int FindFreeVoiceIndex(SoundType type) noexcept;
	PlaybackParams GetParams(const SoundDescriptor* desc);
	float GetGroupVolume(AudioGroupID groupID) const noexcept;
	float GetInstanceVolume(AudioGroupID groupID, float volume) const noexcept;
	Voice& GetVoiceSlot(SoundType type, int index) noexcept;
	const Voice* GetVoice(int handle) const noexcept;
	Voice* GetVoice(int handle) noexcept;
	const ISoundInstance* GetInstance(int handle) const noexcept;
	ISoundInstance* GetInstance(int handle) noexcept;
	ISoundInstance* GetBackendInstance(SoundType type, ISoundBuffer* buffer);

	const SoundAssetView* m_sndAssetView{ nullptr };
	unique_ptr<IAudioBackend> m_audioBackend;
	unique_ptr<SoundRepository> m_repository;
	float m_masterVolume{ 1.0f };
	unordered_map<AudioGroupID, unique_ptr<GroupInfo>> m_groupInfos;

	CycleIterator m_staticCycleIter;
	CycleIterator m_streamCycleIter;
	vector<Voice> m_staticVoices;
	vector<Voice> m_streamVoices;
};