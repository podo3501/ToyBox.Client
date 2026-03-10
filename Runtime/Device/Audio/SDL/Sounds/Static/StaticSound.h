#pragma once

struct MIX_Mixer;
class StaticSoundInstance;
enum class AudioGroupID;
enum class PlayState;
class StaticSound
{
public:
	~StaticSound();
	StaticSound();
	bool Initialize();
	bool LoadSound(string_view soundID, Core::ByteBuffer fileBuffer, AudioGroupID groupID, float volume);
	bool Unload(string_view soundID) noexcept;
	bool Play(string_view soundID) noexcept;
	PlayState GetState(string_view soundID) const noexcept;
	void SetVolume(AudioGroupID groupID, float volume) noexcept;
	bool SetVolume(string_view soundID, float volume) noexcept;

private:
	unordered_map<string, unique_ptr<StaticSoundInstance>> m_instances;
	bool m_init{ false };
	MIX_Mixer* m_mixer{ nullptr };
};
