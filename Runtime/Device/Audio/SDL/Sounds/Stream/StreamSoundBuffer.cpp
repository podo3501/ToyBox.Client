#include "pch.h"
#include "StreamSoundBuffer.h"
#include "Platform/Resource/IResourceStream.h"
#include "Audio/AudioTypes.h"
#include "vorbis/vorbisfile.h"
#include "SDL3/SDL.h"


size_t ReadFunc(void* ptr, size_t size, size_t nmemb, void* datasource)
{
    auto* stream = static_cast<IResourceStream*>(datasource);

    size_t total = size * nmemb;
    span<std::byte> buffer(reinterpret_cast<std::byte*>(ptr), total);

    size_t read = stream->Read(buffer);

    return read / size;
}

int SeekFunc(void* datasource, ogg_int64_t offset, int whence)
{
    auto* stream = static_cast<IResourceStream*>(datasource);

    size_t pos = 0;

    switch (whence)
    {
    case SEEK_SET: pos = offset; break;
    case SEEK_CUR: pos = stream->Tell() + offset; break;
    case SEEK_END: pos = stream->Size() + offset; break;
    }

    return stream->Seek(pos) ? 0 : -1;
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

    if (!vi)
    {
        // 정보 없으면 기본값
        spec.format = SDL_AUDIO_F32; // float 샘플
        spec.channels = 2;       // 기본 스테레오
        spec.freq = 44100;       // 기본 샘플레이트
        return spec;
    }

    spec.channels = vi->channels;
    spec.freq = vi->rate;
    spec.format = SDL_AUDIO_F32; // libvorbis는 일반적으로 16bit signed PCM으로 디코딩되지만, float로도 받을 수 있음. 여기서는 float로 설정

    return spec;
}


StreamSoundBuffer::~StreamSoundBuffer() = default;
StreamSoundBuffer::StreamSoundBuffer() = default;
bool StreamSoundBuffer::Load(unique_ptr<IResourceStream> stream, AudioGroupID groupID, float volume)
{
    OggVorbis_File vf;
    
    ov_callbacks cb{};
    cb.read_func = ReadFunc;
    cb.seek_func = SeekFunc;
    cb.tell_func = TellFunc;
    cb.close_func = CloseFunc;

    if (ov_open_callbacks(stream.get(), &vf, nullptr, 0, cb) < 0) return false;



    //SDL_LoadAudio_IO()
     //int error = 0;

    //m_vorbis = stb_vorbis_open_memory(
    //    buffer.data(),
    //    static_cast<int>(buffer.size()),
    //    &error,
    //    nullptr
    //);

    //if (!m_vorbis)
    //    return false;
    return true;
}