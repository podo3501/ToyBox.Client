#pragma once

struct MIX_Mixer;
class AudioMixer //RAII를 위해서 작은 클래스로 만듦.
{ 
public:
    ~AudioMixer();
    AudioMixer() = default;
    AudioMixer(const AudioMixer&) = delete;
    AudioMixer& operator=(const AudioMixer&) = delete;

    bool Initialize();
    inline MIX_Mixer* Get() const noexcept { return m_mixer; }

private:
    MIX_Mixer* m_mixer{ nullptr }; //믹서 안에 Device가 들어가 있다. 
};
