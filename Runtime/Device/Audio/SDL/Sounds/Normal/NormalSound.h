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
	bool LoadSound(const filesystem::path& filename, AudioGroupID groupID, float volume);
	bool Unload(const filesystem::path& filename) noexcept;
	bool Play(const filesystem::path& filename);
	PlayState GetPlayState(const filesystem::path& filename) const noexcept;
	void SetVolume(AudioGroupID groupID, float volume) noexcept;

private:
	unordered_map<filesystem::path, unique_ptr<NormalSoundBuffer>> m_normalSoundBuffers;
	bool m_init{ false };
	MIX_Mixer* m_mixer{ nullptr };
};
