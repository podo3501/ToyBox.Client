#pragma once

struct MIX_Mixer;
class NormalSoundBuffer;
enum class AudioGroupID;
enum class PlayState;
class NormalSound
{
public:
	~NormalSound();
	NormalSound();
	bool Initialize();
	bool LoadSound(const string& filename, AudioGroupID groupID, float volume);
	bool Unload(const string& filename) noexcept;
	bool Play(const string& filename);
	PlayState GetPlayState(const string& filename) const noexcept;
	void SetVolume(AudioGroupID groupID, float volume) noexcept;

private:
	unordered_map<string, unique_ptr<NormalSoundBuffer>> m_normalSoundBuffers;
	bool m_init{ false };
	MIX_Mixer* m_mixer{ nullptr };
};
