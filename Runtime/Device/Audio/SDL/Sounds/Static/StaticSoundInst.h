#pragma once
#include "SDL3/SDL_properties.h"

struct MIX_Mixer;
struct MIX_Track;
class StaticSoundBuffer;
enum class AudioGroupID;
class StaticSoundInst
{
public:
    ~StaticSoundInst();
    StaticSoundInst();
    bool Setup(MIX_Mixer* mixer);
    bool Reset(StaticSoundBuffer* buffer, AudioGroupID groupID, float volume);
    void Play();
    bool SetVolume(float volume);
    bool IsPlaying() const noexcept;
    AudioGroupID GetGroupID() const noexcept;

private:
    StaticSoundBuffer* m_buffer;
    MIX_Track* m_track;
    AudioGroupID m_groupID;
    SDL_PropertiesID m_options{};
};
