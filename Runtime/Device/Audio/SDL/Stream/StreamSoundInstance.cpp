#include "pch.h"
#include "StreamSoundInstance.h"
#include "StreamSoundBuffer.h"
#include "../AudioDevice.h"
#include "VorbisStreamCallbacks.h"
#include "Platform/Resource/IResourceStream.h"
#include "Audio/AudioTypes.h"

SDL_AudioSpec VorbisToSDLAudioSpec(OggVorbis_File& vf)
{
    SDL_AudioSpec spec{};
    if (auto* vi = ov_info(&vf, -1))
    {
        spec.channels = static_cast<Uint8>(vi->channels);
        spec.freq = static_cast<int>(vi->rate);
    }
    else
    {
        spec.channels = 2;
        spec.freq = 44100;
    }
    spec.format = SDL_AUDIO_S16; // ov_read()는 항상 signed 16bit PCM을 반환

    return spec;
}

StreamSoundInstance::~StreamSoundInstance()
{
    if (m_vorbisOpened)
        ov_clear(&m_vorbisFile);
}
StreamSoundInstance::StreamSoundInstance() = default;

bool StreamSoundInstance::Setup(AudioDevice* device)
{
    m_device = device;
    memset(&m_vorbisFile, 0, sizeof(OggVorbis_File));
    return true;
}

bool StreamSoundInstance::SetBuffer(StreamSoundBuffer* buffer)
{
    if (buffer == nullptr) return false;
    m_buffer = buffer;

    return true;
}

bool StreamSoundInstance::PrepareStream()
{
    m_resourceStream = m_buffer->GetStream()->Clone();

    auto cb = Vorbis::CreateCallbacks();
    int result = ov_open_callbacks(m_resourceStream.get(), &m_vorbisFile, nullptr, 0, cb);
    if (result < 0) return false;
    m_vorbisOpened = true;

    SDL_AudioSpec srcSpec = VorbisToSDLAudioSpec(m_vorbisFile);
    m_stream = m_device->CreateStream(srcSpec);
    if (!m_stream) return false;

    return SDL_BindAudioStream(m_device->Get(), m_stream);
}

bool StreamSoundInstance::Reset(const PlaybackParams& params)
{
    ReturnIfFalse(PrepareStream());

    ov_pcm_seek(&m_vorbisFile, 0);
    SDL_ClearAudioStream(m_stream);
    ReturnIfFalse(SetVolume(params.volume));
    m_loop = params.loop;

    return true;
}

bool StreamSoundInstance::Play()
{
    if (!m_stream) return false;

    m_finished = false;
    constexpr int INITIAL_FILL = 4;
    for (int i = 0; i < INITIAL_FILL; ++i)
        if(!PushChunk()) return false;

    return SDL_ResumeAudioStreamDevice(m_stream);
}

bool StreamSoundInstance::Pause()
{
    if (!m_stream) 
        return false;

    return SDL_PauseAudioStreamDevice(m_stream);
}

bool StreamSoundInstance::Resume()
{
    if (!m_stream)
        return false;

    return SDL_ResumeAudioStreamDevice(m_stream);
}

bool StreamSoundInstance::Stop()
{
    if (!m_stream)
        return false;

    ReturnIfFalse(SDL_ClearAudioStream(m_stream));
    m_finished = true;

    return true;
}

void StreamSoundInstance::Update()
{
    if (!m_stream || m_finished)
        return;

    constexpr int LOW_WATERMARK = 128 * 1024;
    while (SDL_GetAudioStreamQueued(m_stream) < LOW_WATERMARK)
        if (!PushChunk()) break;
}

bool StreamSoundInstance::SetVolume(float volume)
{
    return SDL_SetAudioStreamGain(m_stream, volume);
}

PlaybackState StreamSoundInstance::GetState() const noexcept 
{ 
    if (!m_stream) return EnumUtil::Invalid<PlaybackState>;
    if (SDL_AudioStreamDevicePaused(m_stream)) return PlaybackState::Paused;
    if (!m_finished || SDL_GetAudioStreamQueued(m_stream) > 0) return PlaybackState::Playing;

    return PlaybackState::Stopped;
}

bool StreamSoundInstance::PushChunk()
{
    if (!m_stream || !m_vorbisOpened) return false;
  
    int bitstream = 0;
    long bytes = ov_read(&m_vorbisFile, m_decodeBuffer.data(), static_cast<int>(m_decodeBuffer.size()),
        0, // little endian (window에서는 리틀엔디언) 0x1234 가 34 12로 돼 있는 방식
        2, // 샘플크기 16비트 PCM
        1, // 0 = unsigned, 1 = signed. 16bit PCM은 일반적으로 signed를 사용
        &bitstream);

    if (bytes > 0)
    {
        SDL_PutAudioStreamData(m_stream, m_decodeBuffer.data(), bytes);
        return true;
    }

    if (bytes == 0)
    {
        if (m_loop)
            ov_pcm_seek(&m_vorbisFile, 0); // EOF (loop 원하면 사용)
        else
        {
            m_finished = true;
            SDL_FlushAudioStream(m_stream);
        }

        return false;
    }

    //bytes < 0 -> 디코딩 오류시 처리.
    m_finished = true;
    SDL_FlushAudioStream(m_stream);
    return false;
}
