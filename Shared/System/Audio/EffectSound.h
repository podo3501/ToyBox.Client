#pragma once

class EffectSoundBuffer;
enum class AudioGroupID;
enum class PlayState;
class EffectSound
{
public:
	~EffectSound();
	EffectSound();
	bool Initialize();
	bool LoadWav(const string& filename, AudioGroupID groupID, float volume);
	bool Unload(const string& filename) noexcept;
	void SetVolume(AudioGroupID groupID, float volume) noexcept;
	bool Play(const string& filename);
	PlayState GetPlayState(const string& filename) const noexcept;
	void Update() noexcept;

private:
	unordered_map<string, EffectSoundBuffer> m_effectSoundBuffers;
};