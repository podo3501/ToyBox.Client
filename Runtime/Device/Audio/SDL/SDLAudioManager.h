#pragma once
#include "../IAudioManager.h"
#include "Shared/Foundation/NoCopyNoMove.h"

struct AudioGroup;
class SoundTableReader;
class EffectSound;
class NormalSound;
class SDLAudioManager : public IAudioManager, private NoCopyNoMove
{
public:
	~SDLAudioManager();
	SDLAudioManager() = delete;
	SDLAudioManager(unique_ptr<IJsonStorage> storage);
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
	string GetFullFilename(const string& index) const noexcept;

	unique_ptr<SoundTableReader> m_reader;
	unique_ptr<EffectSound> m_effectSound;
	unique_ptr<NormalSound> m_normalSound;
	unordered_map<AudioGroupID, unique_ptr<AudioGroup>> m_audioGroups;
};