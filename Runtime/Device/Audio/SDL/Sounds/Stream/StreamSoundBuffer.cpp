#include "pch.h"
#include "StreamSoundBuffer.h"
#include "Platform/Resource/IResourceStream.h"
#include "Audio/AudioTypes.h"

size_t ReadFunc(void* ptr, size_t size, size_t nmemb, void* datasource)
{
    auto* stream = static_cast<IResourceStream*>(datasource);
    size_t bytesRequested = size * nmemb;

    span<std::byte> buffer(reinterpret_cast<std::byte*>(ptr), bytesRequested);
    size_t bytesRead = stream->Read(buffer);

    return bytesRead / size;
}

int SeekFunc(void* datasource, ogg_int64_t offset, int whence)
{
    auto* stream = static_cast<IResourceStream*>(datasource);

    int64_t pos = 0;
    switch (whence)
    {
    case SEEK_SET: pos = offset; break;
    case SEEK_CUR:
        pos = static_cast<int64_t>(stream->Tell()) + offset; break;  case SEEK_END:
        pos = static_cast<int64_t>(stream->Size()) + offset;
        break;
    default:
        return -1;
    }
    if (pos < 0) return -1;

    return stream->Seek(static_cast<size_t>(pos)) ? 0 : -1;
}

long TellFunc(void* datasource)
{
    auto* stream = static_cast<IResourceStream*>(datasource);
    return static_cast<long>(stream->Tell());
}

int CloseFunc(void*)
{
    return 0; // 스트림 소유권은 외부
}

SDL_AudioSpec CreateSDLAudioSpec(OggVorbis_File& vf)
{
    SDL_AudioSpec spec{};
    vorbis_info* vi = ov_info(&vf, -1); // -1은 current logical bitstream

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

StreamSoundBuffer::~StreamSoundBuffer()
{
    if (m_vorbisOpened)
        ov_clear(&m_vorbisFile);
}
StreamSoundBuffer::StreamSoundBuffer()
{
    memset(&m_vorbisFile, 0, sizeof(OggVorbis_File));
}

bool StreamSoundBuffer::Load(SDL_AudioDeviceID device, 
    unique_ptr<IResourceStream> fileStream, AudioGroupID groupID, float volume, bool loop)
{
    m_fileStream = move(fileStream);
    m_loop = loop;
    
    ov_callbacks cb{};
    cb.read_func = ReadFunc;
    cb.seek_func = SeekFunc;
    cb.tell_func = TellFunc;
    cb.close_func = CloseFunc;

    int result = ov_open_callbacks(m_fileStream.get(), &m_vorbisFile, nullptr, 0, cb);
    if (result < 0) return false;
    m_vorbisOpened = true;

    SDL_AudioSpec srcSpec = CreateSDLAudioSpec(m_vorbisFile);

    SDL_AudioSpec deviceSpec{};
    deviceSpec.freq = 48000;
    deviceSpec.format = SDL_AUDIO_S16;
    deviceSpec.channels = 2;

    m_stream = SDL_CreateAudioStream(&srcSpec, &deviceSpec);
    if (!m_stream) return false;

    ReturnIfFalse(SDL_BindAudioStream(device, m_stream));
    ReturnIfFalse(SetVolume(volume));

    return true;
}

void StreamSoundBuffer::Play()
{
    if (!m_stream) return;

    m_finished = false;
    constexpr int INITIAL_FILL = 4;
    for (int i = 0; i < INITIAL_FILL; ++i)
        if(!PushChunk()) return;

    SDL_ResumeAudioStreamDevice(m_stream);
}

void StreamSoundBuffer::Stop()
{
    if (!m_stream)
        return;

    SDL_ClearAudioStream(m_stream);
    m_finished = true;
}

bool StreamSoundBuffer::PushChunk()
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

bool StreamSoundBuffer::IsPlaying() const noexcept
{
    if (!m_stream) return false;
    if (!m_finished) return true;
    return SDL_GetAudioStreamQueued(m_stream) > 0;
}

bool StreamSoundBuffer::SetVolume(float volume)
{
    return SDL_SetAudioStreamGain(m_stream, volume);
}

void StreamSoundBuffer::Update() noexcept
{
    if (!m_stream || m_finished)
        return;

    constexpr int LOW_WATERMARK = 128 * 1024;
    while (SDL_GetAudioStreamQueued(m_stream) < LOW_WATERMARK)
        if (!PushChunk()) break;
}