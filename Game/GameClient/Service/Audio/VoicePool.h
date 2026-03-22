#pragma once
#include "Voice.h"

struct Voice;
struct ISoundInstance;
struct SoundDescriptor;
enum class SoundType;

class VoicePool
{
public:
	~VoicePool();
	VoicePool();
	bool Setup(int maxVoices, int maxStreams) noexcept;
	VoiceHandle AcquireVoice(SoundHandle sh, ISoundInstance* instance, const SoundDescriptor* desc) noexcept;
	bool StopVoice(VoiceHandle vh) noexcept;
	bool StopVoices(SoundHandle sv) noexcept;
	bool StopVoices(SoundType type) noexcept;
	void UpdateVoices() noexcept;

	const Voice* GetVoice(VoiceHandle vh) const noexcept;
	Voice* GetVoice(VoiceHandle vh) noexcept;
	const ISoundInstance* GetInstance(VoiceHandle vh) const noexcept;
	ISoundInstance* GetInstance(VoiceHandle vh) noexcept;

private:
	int m_maxStreams{ 0 };
	HandleAllocator<VoiceTag, 64> m_allocator{ 0 }; //?!? constexpr에 64가 생길 예정.
	vector<Voice> m_voices;
	vector<Voice*> m_stealCandidates;
};
