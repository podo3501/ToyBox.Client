#pragma once
#include <string>
#include <memory>
#include "AudioTypes.h"

class SoundTableReader;
struct IAudioManager
{
	virtual ~IAudioManager() = default;
	virtual bool LoadSound(const std::string& index) = 0;
	virtual bool Unload(const std::string& index) noexcept = 0;
	virtual void SetVolume(AudioGroupID groupID, float volume) noexcept = 0;
	virtual bool Play(const std::string& index) = 0;
	virtual PlayState GetPlayState(const std::string& index) = 0;
	virtual void Update() noexcept = 0;
	//normal sound에 효과(반복, 페이드인 같은)를 주기 위한 함수를 만들어야 하는데, 현재는 필요 없어서 추후에 만들기로 한다.
};

unique_ptr<IAudioManager> CreateAudioManager(SoundTableReader reader);