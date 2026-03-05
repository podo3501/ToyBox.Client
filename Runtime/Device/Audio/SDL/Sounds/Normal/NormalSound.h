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
	bool LoadSound(string_view soundID, Core::ByteBuffer fileBuffer, AudioGroupID groupID, float volume);
	bool Unload(string_view soundID) noexcept;
	bool Play(string_view soundID) noexcept;
	PlayState GetState(string_view soundID) const noexcept;
	void SetVolume(AudioGroupID groupID, float volume) noexcept;

private:
	unordered_map<string, unique_ptr<NormalSoundBuffer>> m_normalSoundBuffers;
	bool m_init{ false };
	MIX_Mixer* m_mixer{ nullptr };
};
