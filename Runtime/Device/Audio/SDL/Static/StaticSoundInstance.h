#pragma once
#include "SDL3/SDL_properties.h"
#include "GameClient/Service/Audio/ISoundInstance.h"

struct MIX_Mixer;
struct MIX_Track;
class StaticSoundBuffer;
class StaticSoundInstance : public ISoundInstance
{
public:
    ~StaticSoundInstance();
    StaticSoundInstance();
    virtual bool Reset(const PlaybackParams& params) override;
    virtual bool Play() override;
    virtual bool Pause() override;
    virtual bool Resume() override;
    virtual bool Stop() override;
    virtual void Update() override;
    virtual bool SetVolume(float volume) override;
    virtual PlaybackState GetState() const noexcept override;

    bool Setup(MIX_Mixer* mixer);
    bool SetBuffer(StaticSoundBuffer* buffer);
    void OnStopped(); //callback ÇÔ¼ö

private:
    MIX_Track* m_track{ nullptr };
    SDL_PropertiesID m_options{};
    PlaybackState m_state{ PlaybackState::Stopped };
};
