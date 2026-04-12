#pragma once
#include "GameClient/Service/Audio/ISoundBuffer.h"

struct MIX_Mixer;
struct MIX_Audio;
class StaticSoundBuffer : public IStaticSoundBuffer
{
public:
    ~StaticSoundBuffer();
    StaticSoundBuffer() = delete;
    explicit StaticSoundBuffer(MIX_Mixer* mixer) noexcept;
    virtual bool LoadFromMemory(Core::ByteBuffer fileBuffer) override;
    virtual bool LoadFromAsset(shared_ptr<StaticSoundAsset> asset) override;
    inline MIX_Audio* GetAudio() const noexcept { return m_audio; }

private:
    MIX_Mixer* m_mixer;
    MIX_Audio* m_audio;
};
