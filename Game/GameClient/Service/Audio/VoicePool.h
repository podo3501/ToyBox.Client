#pragma once
#include "Voice.h"
#include "Core/Utils/Handle/FixedHandlePool.h"

struct Voice;
struct IAudioBackend;
struct ISoundInstance;
struct SoundDesc;
struct LoadedSound;
struct PlaybackParams;
enum class SoundType;
enum class PlaybackState;

class VoicePool
{
public:
	~VoicePool();
	VoicePool(IAudioBackend* audioBackend);
	bool Setup(int maxVoices, int maxStreams) noexcept;
	VoiceHandle AcquireVoiceHandle(const SoundDesc* desc) noexcept;
	VoiceHandle Play(SoundHandle sh, const LoadedSound* loaded, const PlaybackParams& params) noexcept;
	bool Play(VoiceHandle vh, SoundHandle sh, const LoadedSound* loaded, const PlaybackParams& params) noexcept;
	bool Pause(VoiceHandle vh) noexcept;
	bool Resume(VoiceHandle vh) noexcept;
	bool StopVoice(VoiceHandle vh) noexcept;
	bool StopVoices(SoundHandle sh) noexcept; //동일한 데이터를 가진거 전부 스톱
	bool StopVoices(SoundType type) noexcept; // 동일한 타입 전부 스톱
	bool SetVolume(VoiceHandle vh, float volume) noexcept;
	PlaybackState GetState(VoiceHandle vh) const noexcept;
	void UpdateVoices() noexcept;
	const SoundDesc* GetDesc(VoiceHandle vh) const noexcept;

private:
	ISoundInstance* CreateInstance(const LoadedSound* loaded);
	void ActivateVoice(VoiceHandle vh, SoundHandle sh, ISoundInstance* instance, const SoundDesc* desc) noexcept;
	VoiceHandle StealAndAcquire(const SoundDesc* desc, vector<Voice*>& stealList) noexcept;

	const ISoundInstance* GetInstance(VoiceHandle vh) const noexcept;
	ISoundInstance* GetInstance(VoiceHandle vh) noexcept;

	IAudioBackend* m_audioBackend{ nullptr };
	int m_maxVoices{ 0 };
	int m_maxStreams{ 0 };
	static const int MaxHandles{ 64 }; //핸들의 최종 크기. Setup시 voice 크기가 더 크면 오류.

	FixedHandlePool<Voice, VoiceTag, MaxHandles> m_voices;
	vector<Voice*> m_stealStatics;
	vector<Voice*> m_stealStreams;
};
