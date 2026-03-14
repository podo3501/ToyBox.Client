#pragma once
#include "SoundTable.h"
#include "Core/Utils/CycleIterator.h"

struct IAudioBackend;
struct IResourceManager;
struct GroupInfo;
struct PlayingSound;
struct ISoundInstance;
struct Voice;
struct ISoundBuffer;

struct LoadedSound
{
	const SoundInfo* info;
	shared_ptr<ISoundBuffer> buffer;
};

class AudioService
{
public:
	~AudioService();
	AudioService() = delete;
	static unique_ptr<AudioService> Create(SoundTable sndTable, unique_ptr<IAudioBackend> backend,
		IResourceManager* resManager, int maxVoices) noexcept;
	int LoadSound(string_view soundID);
	bool Unload(int soundHandle) noexcept;
	int Play(int soundHandle) noexcept;
	bool Pause(int instanceHandle) noexcept;
	bool Stop(int instanceHandle) noexcept;
	void Update() noexcept;
	PlayState GetState(int handle) const noexcept;
	inline void SetMasterVolume(float volume) noexcept { m_masterVolume = volume; }
	inline float GetMasterVolume() const noexcept { return m_masterVolume; }
	bool SetVolume(int handle, float volume) noexcept;

private:
	AudioService(SoundTable sndTable, unique_ptr<IAudioBackend> audioBackend,
		IResourceManager* resManager) noexcept;
	bool Initialize(int maxVoices) noexcept;
	shared_ptr<ISoundBuffer> CreateSoundBuffer(const SoundInfo* info);
	void CreateAudioGroup() noexcept;
	int FindFreeVoiceIndex() noexcept;
	float GetGroupVolume(AudioGroupID groupID) const noexcept;
	AudioGroupID GetGroupID(string_view soundID);
	const ISoundInstance* GetInstance(int handle) const noexcept;
	ISoundInstance* GetInstance(int handle) noexcept;

	const Voice* GetVoice(int handle) const noexcept;
	Voice* GetVoice(int handle) noexcept;

	SoundTable m_sndTable;
	unique_ptr<IAudioBackend> m_audioBackend;
	IResourceManager* m_resManager{ nullptr };
	unordered_map<filesystem::path, weak_ptr<ISoundBuffer>> m_buffers;
	unordered_map<int, LoadedSound> m_loadedSounds;
	int m_nextSoundHandle{ 1 };
	float m_masterVolume{ 1.0f };
	unordered_map<AudioGroupID, unique_ptr<GroupInfo>> m_groupInfos;

	CycleIterator m_cycleIter;
	vector<Voice> m_voices;
};