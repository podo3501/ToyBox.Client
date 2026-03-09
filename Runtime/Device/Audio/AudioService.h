#pragma once
#include "SoundTable.h"

struct IAudioBackend;
struct IResourceManager;
struct GroupInfo;
class AudioService
{
public:
	~AudioService();
	AudioService() = delete;
	AudioService(SoundTable sndTable, 
		unique_ptr<IAudioBackend> audioBackend,
		IResourceManager* resManager) noexcept;

	bool Load(string_view soundID);
	bool Unload(string_view soundID) noexcept;
	bool Play(string_view soundID) noexcept;
	void Update() noexcept;
	PlayState GetState(string_view soundID) const noexcept;
	inline void SetMasterVolume(float volume) noexcept { m_masterVolume = volume; }
	inline float GetMasterVolume() const noexcept { return m_masterVolume; }
	bool SetVolume(string_view soundID, float volume) noexcept;

private:
	void CreateAudioGroup() noexcept;
	float GetGroupVolume(AudioGroupID groupID) const noexcept;
	AudioGroupID GetGroupID(string_view soundID);

	SoundTable m_sndTable;
	unique_ptr<IAudioBackend> m_audioBackend;
	IResourceManager* m_resManager{ nullptr };
	float m_masterVolume{ 1.0f };
	unordered_map<AudioGroupID, unique_ptr<GroupInfo>> m_groupInfos;
};