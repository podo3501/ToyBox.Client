#pragma once
#include "SoundTable.h"
#include "Core/Utils/CycleIterator.h"

struct IAudioBackend;
struct IResourceManager;
struct GroupInfo;
struct PlayingSound;
struct ISoundInstance;
class ISoundBuffer;

struct LoadedSound
{
	const SoundInfo* info;
	shared_ptr<ISoundBuffer> buffer;
};

struct Voice
{
	bool active{ false };
	uint32_t generation{ 0 };
	ISoundInstance* instance{ nullptr };
};

class AudioService
{
public:
	~AudioService();
	AudioService() = delete;
	static unique_ptr<AudioService> Create(SoundTable sndTable, unique_ptr<IAudioBackend> backend,
		IResourceManager* resManager, int maxVoices) noexcept;
	bool Load(string_view soundID);
	int LoadSound(string_view soundID);
	bool Unload(string_view soundID) noexcept;
	bool Unload(int soundHandle) noexcept;
	int Play(int soundHandle) noexcept;
	bool Play(string_view soundID) noexcept;
	//bool Play(int handle) noexcept;
	void Stop(int instanceHandle) noexcept;
	void Update() noexcept;
	PlayState GetState(string_view soundID) const noexcept;
	PlayState GetState(int handle) const noexcept;
	inline void SetMasterVolume(float volume) noexcept { m_masterVolume = volume; }
	inline float GetMasterVolume() const noexcept { return m_masterVolume; }
	bool SetVolume(string_view soundID, float volume) noexcept;
	bool SetVolumE(int handle, float volume) noexcept;

private:
	AudioService(SoundTable sndTable, unique_ptr<IAudioBackend> audioBackend,
		IResourceManager* resManager) noexcept;
	bool Initialize(int maxVoices) noexcept;
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
	unordered_map<int, PlayingSound> m_playingSounds;

	CycleIterator m_cycleIter;
	vector<Voice> m_voices;
};