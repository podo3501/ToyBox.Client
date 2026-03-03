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
	bool LoadWav(const filesystem::path& filename, AudioGroupID groupID, float volume);
	bool Unload(const filesystem::path& filename) noexcept;
	void SetVolume(AudioGroupID groupID, float volume) noexcept;
	bool Play(const filesystem::path& filename);
	PlayState GetPlayState(const filesystem::path& filename) const noexcept;
	void Update() noexcept;

private:
	unordered_map<filesystem::path, EffectSoundBuffer> m_effectSoundBuffers;
};