#pragma once
#include "../IAudioBackend.h"

class NormalSound;
class StreamSound;
class SDLAudioBackend : public IAudioBackend
{
public:
	~SDLAudioBackend();
	SDLAudioBackend();
	bool Initialize() noexcept;
	virtual bool LoadPreload(string_view soundID, Core::ByteBuffer buffer, AudioGroupID groupID, float volume) override;
	virtual bool LoadStream(string_view soundID, unique_ptr<IResourceStream> stream, AudioGroupID groupID, float volume) override;
	virtual bool Unload(string_view soundID) noexcept override;
	virtual bool Play(string_view soundID) noexcept override;
	virtual void Update() noexcept override;
	virtual PlayState GetState(string_view soundID) const noexcept override;

private:
	unique_ptr<NormalSound> m_normalSound;
	unique_ptr<StreamSound> m_streamSound;
};


