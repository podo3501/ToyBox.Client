#pragma once
#include "../IAudioBackend.h"

class EffectSound;
class NormalSound;
class SDLAudioBackend : public IAudioBackend
{
public:
	~SDLAudioBackend();
	SDLAudioBackend();
	bool Initialize() noexcept;
	virtual bool Load(string_view soundID, Core::ByteBuffer buffer, AudioGroupID groupID, float volume) override;
	virtual bool Play(string_view soundID) noexcept override;
	virtual void Update() noexcept override;
	virtual PlayState GetState(string_view soundID) const noexcept override;

private:
	unique_ptr<EffectSound> m_effectSound;
	unique_ptr<NormalSound> m_normalSound;
};


