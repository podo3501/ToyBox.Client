#pragma once
#include "SoundHandle.h"
#include "VoiceHandle.h"

struct IAudioBackend;
struct IResourceManager;
struct SoundAssetView;
struct GroupInfo;
struct PlaybackParams;
struct SoundDescriptor;
class SoundRepository;
class VoicePool;
enum class PlaybackState;
enum class AudioGroup;

class AudioService
{
public:
	~AudioService();
	AudioService() = delete;
	static unique_ptr<AudioService> Create(const SoundAssetView* sndAssetView, unique_ptr<IAudioBackend> backend,
		IResourceManager* resManager, int maxVoices, int maxStreams) noexcept;
	SoundHandle LoadStaticSound(string_view soundID);
	SoundHandle LoadStreamSound(string_view soundID);
	VoiceHandle Play(SoundHandle sh) noexcept;
	bool Pause(VoiceHandle vh) noexcept;
	bool Resume(VoiceHandle vh) noexcept;
	bool Stop(VoiceHandle vh) noexcept;
	bool AllStop() noexcept;
	void Update() noexcept;
	bool Unload(SoundHandle h) noexcept;
	PlaybackState GetState(VoiceHandle vh) const noexcept;
	inline void SetMasterVolume(float volume) noexcept { m_masterVolume = volume; }
	inline float GetMasterVolume() const noexcept { return m_masterVolume; }
	bool SetVolume(VoiceHandle vh, float volume) noexcept;

private:
	AudioService(const SoundAssetView* sndAssetView, unique_ptr<SoundRepository> sndRepository,
		unique_ptr<IAudioBackend> audioBackend) noexcept;
	bool Initialize(int maxVoices, int maxStreams) noexcept;
	void CreateAudioGroup() noexcept;
	PlaybackParams GetParams(const SoundDescriptor* desc) noexcept;
	float GetGroupVolume(AudioGroup group) const noexcept;
	float GetInstanceVolume(AudioGroup group, float volume) const noexcept;

	const SoundAssetView* m_sndAssetView{ nullptr };
	unique_ptr<IAudioBackend> m_audioBackend;
	unique_ptr<SoundRepository> m_repository;
	unique_ptr<VoicePool> m_voicePool;
	float m_masterVolume{ 1.0f };
	unordered_map<AudioGroup, unique_ptr<GroupInfo>> m_groupInfos;
};