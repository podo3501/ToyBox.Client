#pragma once
#include "StaticSoundTable.h"
#include "StreamSoundTable.h"
#include "Core/Utils/CycleIterator.h"

struct IAudioBackend;
struct IResourceManager;
struct ISoundBuffer;
struct ISoundInstance;
struct GroupInfo;
struct LoadedSound;
struct Voice;
struct PlaybackParams;
class AudioService
{
public:
	~AudioService();
	AudioService() = delete;
	static unique_ptr<AudioService> Create(StaticSoundTable staticTable, StreamSoundTable streamTable,
		unique_ptr<IAudioBackend> backend, IResourceManager* resManager, int maxVoices, int maxStreams) noexcept;
	int LoadStaticSound(string_view soundID);
	int LoadStreamSound(string_view soundID);
	bool Unload(int soundHandle) noexcept;
	int Play(int soundHandle) noexcept;
	bool Pause(int instanceHandle) noexcept;
	bool Resume(int instanceHandle) noexcept;
	bool Stop(int instanceHandle) noexcept;
	void Update() noexcept;
	PlaybackState GetState(int handle) const noexcept;
	inline void SetMasterVolume(float volume) noexcept { m_masterVolume = volume; }
	inline float GetMasterVolume() const noexcept { return m_masterVolume; }
	bool SetVolume(int handle, float volume) noexcept;

private:
	AudioService(StaticSoundTable staticTable, StreamSoundTable streamTable, 
		unique_ptr<IAudioBackend> audioBackend, IResourceManager* resManager) noexcept;
	bool Initialize(int maxVoices, int maxStreams) noexcept;
	shared_ptr<ISoundBuffer> CreateStaticSoundBuffer(const StaticSoundInfo* info);
	shared_ptr<ISoundBuffer> CreateStreamSoundBuffer(const StreamSoundInfo* info);
	template<typename InfoType, typename CreateFunc>
	int LoadSoundInternal(const InfoType* info, CreateFunc createFunc);
	void CreateAudioGroup() noexcept;
	int FindFreeVoiceIndex(SoundType type) noexcept;
	PlaybackParams GetParams(const SoundInfo* info);
	float GetGroupVolume(AudioGroupID groupID) const noexcept;
	float GetInstanceVolume(AudioGroupID groupID, float volume) const noexcept;
	Voice& GetVoiceSlot(SoundType type, int index) noexcept;
	const Voice* GetVoice(int handle) const noexcept;
	Voice* GetVoice(int handle) noexcept;
	const ISoundInstance* GetInstance(int handle) const noexcept;
	ISoundInstance* GetInstance(int handle) noexcept;

	StaticSoundTable m_staticTable;
	StreamSoundTable m_streamTable;
	unique_ptr<IAudioBackend> m_audioBackend;
	IResourceManager* m_resManager{ nullptr };
	unordered_map<filesystem::path, weak_ptr<ISoundBuffer>> m_buffers;
	unordered_map<int, LoadedSound> m_loadedSounds;
	int m_nextSoundHandle{ 1 };
	float m_masterVolume{ 1.0f };
	unordered_map<AudioGroupID, unique_ptr<GroupInfo>> m_groupInfos;

	CycleIterator m_staticCycleIter;
	CycleIterator m_streamCycleIter;
	vector<Voice> m_staticVoices;
	vector<Voice> m_streamVoices;
};