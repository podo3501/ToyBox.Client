#pragma once
#include "SoundHandle.h"
#include "VoiceHandle.h"

struct IAudioBackend;
struct SoundAssetView;
struct StaticSoundAsset;
struct StreamSoundAsset;
struct GroupInfo;
struct PlaybackParams;
struct SoundDesc;
struct PendingSoundPlay;
class AssetPipeline;
class SoundRepository;
class VoicePool;
enum class PlaybackState;
enum class AudioGroup;

class AudioService
{
public:
	~AudioService();
	AudioService() = delete;
	static unique_ptr<AudioService> Create(const SoundAssetView& sndAssetView, unique_ptr<IAudioBackend> backend,
		AssetPipeline* assetPipeline, int maxVoices, int maxStreams) noexcept;
	SoundHandle AcquireStaticSound(string_view soundID);
	SoundHandle AcquireStreamSound(string_view soundID);
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
	AudioService(
		const SoundAssetView& sndAssetView, 
		unique_ptr<IAudioBackend> audioBackend, 
		AssetPipeline* assetPipeline) noexcept;
	bool Initialize(int maxVoices, int maxStreams) noexcept;
	void CreateAudioGroup() noexcept;
	PlaybackParams GetParams(const SoundDesc* desc) noexcept;
	VoiceHandle EnqueueDeferred(SoundHandle sh, const SoundDesc* desc);
	void FlushPending();
	float GetGroupVolume(AudioGroup group) const noexcept;
	float GetInstanceVolume(AudioGroup group, float volume) const noexcept;

	unique_ptr<SoundAssetView> m_sndAssetView;
	unique_ptr<IAudioBackend> m_audioBackend;
	unique_ptr<SoundRepository> m_repository;
	unique_ptr<VoicePool> m_voicePool;
	float m_masterVolume{ 1.0f };
	std::unordered_map<AudioGroup, unique_ptr<GroupInfo>> m_groupInfos;
	std::vector<PendingSoundPlay> m_pendingPlay;
};