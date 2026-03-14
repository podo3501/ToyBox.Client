#include "pch.h"
#include "StreamSoundInstance.h"
#include "AudioDevice.h"
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
StreamSoundInstance::StreamSoundInstance()
    : m_groupID{ EnumUtil::Invalid<AudioGroupID> }
{
    memset(&m_vorbisFile, 0, sizeof(OggVorbis_File));
}

bool StreamSoundInstance::Load(AudioDevice* device,
    unique_ptr<IResourceStream> fileStream, AudioGroupID groupID, float volume, bool loop)
{
    m_fileStream = move(fileStream);
    m_loop = loop;
    m_groupID = groupID;

    auto cb = Vorbis::CreateCallbacks();
    int result = ov_open_callbacks(m_fileStream.get(), &m_vorbisFile, nullptr, 0, cb);
    if (result < 0) return false;
    m_vorbisOpened = true;

    SDL_AudioSpec srcSpec = VorbisToSDLAudioSpec(m_vorbisFile);
    m_stream = device->CreateStream(srcSpec);
    if (!m_stream) return false;

    ReturnIfFalse(SDL_BindAudioStream(device->GetDevice(), m_stream));
    ReturnIfFalse(SetVolume(volume));

    return true;
}

void StreamSoundInstance::Play()
{
    if (!m_stream) return;

    m_finished = false;
    constexpr int INITIAL_FILL = 4;
    for (int i = 0; i < INITIAL_FILL; ++i)
        if(!PushChunk()) return;

    SDL_ResumeAudioStreamDevice(m_stream);
}

void StreamSoundInstance::Stop()
{
    if (!m_stream)
        return;

    SDL_ClearAudioStream(m_stream);
    m_finished = true;
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

bool StreamSoundInstance::IsPlaying() const noexcept
{
    if (!m_stream) return false;
    if (!m_finished) return true;
    return SDL_GetAudioStreamQueued(m_stream) > 0;
}

bool StreamSoundInstance::SetVolume(float volume)
{
    return SDL_SetAudioStreamGain(m_stream, volume);
}

void StreamSoundInstance::Update() noexcept
{
    if (!m_stream || m_finished)
        return;

    constexpr int LOW_WATERMARK = 128 * 1024;
    while (SDL_GetAudioStreamQueued(m_stream) < LOW_WATERMARK)
        if (!PushChunk()) break;
}

AudioGroupID StreamSoundInstance::GetGroupID() const noexcept { return m_groupID; }