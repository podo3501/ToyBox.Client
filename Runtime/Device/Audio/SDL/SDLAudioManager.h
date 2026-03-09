#pragma once
#include "../IAudioManager.h"
#include "../SoundTable.h"

struct AudioGroup;
class SoundTable;
class EffectSound;
class NormalSound;
class SDLAudioManager : public IAudioManager, private NoCopyNoMove
{
public:
	~SDLAudioManager();
	SDLAudioManager() = delete;
	SDLAudioManager(SoundTable sndTable);
	bool Initialize();
	virtual bool LoadSound(const string& index) override;
	virtual bool Unload(const string& index) noexcept override;
	virtual void SetVolume(AudioGroupID groupID, float volume) noexcept override;
	virtual bool Play(const string& index) override;
	virtual PlayState GetPlayState(const string& index) override;
	virtual void Update() noexcept override;

private:
	void CreateAudioGroup() noexcept;
	float GetVolume(AudioGroupID groupID) const noexcept;
	filesystem::path GetFullFilename(const string& index) const noexcept;

	SoundTable m_sndTable;
	unique_ptr<EffectSound> m_effectSound;
	unique_ptr<NormalSound> m_normalSound;

	float m_masterVolume{ 1.0f };
	unordered_map<AudioGroupID, unique_ptr<AudioGroup>> m_audioGroups;
};