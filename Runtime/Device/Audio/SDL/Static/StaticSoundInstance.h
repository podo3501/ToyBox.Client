#pragma once
#include "SDL3/SDL_properties.h"
#include "Device/Audio/ISoundInstance.h"

struct MIX_Mixer;
struct MIX_Track;
class StaticSoundBuffer;
enum class AudioGroupID;
class StaticSoundInstance : public ISoundInstance
{
public:
    ~StaticSoundInstance();
    StaticSoundInstance();
    virtual bool Reset(float volume) override;
    virtual bool Play() override;
    virtual bool Pause() override;
    virtual bool Stop() override;
    virtual bool SetVolume(float volume) override;
    virtual bool IsPlaying() const noexcept override;
    virtual ISoundBuffer* GetBuffer() noexcept override;

    bool Setup(MIX_Mixer* mixer);
    bool SetBuffer(StaticSoundBuffer* buffer);
    AudioGroupID GetGroupID() const noexcept;

private:
    StaticSoundBuffer* m_buffer;
    MIX_Track* m_track;
    SDL_PropertiesID m_options{};
};
