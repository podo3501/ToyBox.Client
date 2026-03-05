#pragma once
#include "SoundTable.h"

struct IAudioBackend;
struct IResourceManager;
struct AudioGroup;
class AudioService
{
public:
	~AudioService();
	AudioService() = delete;
	AudioService(SoundTable sndTable, 
		unique_ptr<IAudioBackend> audioBackend,
		IResourceManager* resManager) noexcept;

	bool Load(string_view soundID);
	bool Play(string_view soundID) noexcept;
	void Update() noexcept;
	PlayState GetState(string_view soundID) const noexcept;

private:
	void CreateAudioGroup() noexcept;
	float GetVolume(AudioGroupID groupID) const noexcept;

	SoundTable m_sndTable;
	unique_ptr<IAudioBackend> m_audioBackend;
	IResourceManager* m_resManager{ nullptr };
	unordered_map<AudioGroupID, unique_ptr<AudioGroup>> m_audioGroups;
};