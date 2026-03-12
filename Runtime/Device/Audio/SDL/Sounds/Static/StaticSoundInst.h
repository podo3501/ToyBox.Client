#pragma once
#include "SDL3/SDL_properties.h"
#include "Device/Audio/ISoundInstance.h"

struct MIX_Mixer;
struct MIX_Track;
class StaticSoundBuffer;
enum class AudioGroupID;
class StaticSoundInst : public ISoundInstance
{
public:
    ~StaticSoundInst();
    StaticSoundInst();
    virtual bool Play() override;
    virtual bool Reset(float volume) override;
    virtual bool IsPlaying() const noexcept override;

    bool Setup(MIX_Mixer* mixer);
    bool SetBuffer(StaticSoundBuffer* buffer);
    bool Stop();
    bool SetVolume(float volume);
    AudioGroupID GetGroupID() const noexcept;

private:
    StaticSoundBuffer* m_buffer;
    MIX_Track* m_track;
    AudioGroupID m_groupID;
    SDL_PropertiesID m_options{};
};
