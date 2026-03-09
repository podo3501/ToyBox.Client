#include "pch.h"
#include "VorbisStreamCallbacks.h"
#include "Platform/Resource/IResourceStream.h"

ov_callbacks Vorbis::CreateCallbacks()
{
    ov_callbacks cb{};
    cb.read_func = Vorbis::ReadFunc;
    cb.seek_func = Vorbis::SeekFunc;
    cb.tell_func = Vorbis::TellFunc;
    cb.close_func = Vorbis::CloseFunc;

    return cb;
}

size_t Vorbis::ReadFunc(void* ptr, size_t size, size_t nmemb, void* datasource)
{
    auto* stream = static_cast<IResourceStream*>(datasource);
    size_t bytesRequested = size * nmemb;

    span<std::byte> buffer(reinterpret_cast<std::byte*>(ptr), bytesRequested);
    size_t bytesRead = stream->Read(buffer);

    return bytesRead / size;
}

int Vorbis::SeekFunc(void* datasource, ogg_int64_t offset, int whence)
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

long Vorbis::TellFunc(void* datasource)
{
    auto* stream = static_cast<IResourceStream*>(datasource);
    return static_cast<long>(stream->Tell());
}

int Vorbis::CloseFunc(void*)
{
    return 0; // 스트림 소유권은 외부
}