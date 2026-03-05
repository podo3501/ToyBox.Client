#include "pch.h"
#include "AudioFormat.h"

static bool HasSignature(const Core::ByteBuffer& buffer, size_t offset, const char* sig, size_t sigSize)
{
    if (buffer.size() < offset + sigSize)
        return false;

    return memcmp(buffer.data() + offset, sig, sigSize) == 0;
}

static bool IsOgg(const Core::ByteBuffer& buffer)
{
    if (buffer.size() < 4)
        return false;

    return HasSignature(buffer, 0, "OggS", 4);
}

static bool IsWav(const Core::ByteBuffer& buffer)
{
    if (buffer.size() < 12)
        return false;

    // "RIFF" + "WAVE"
    return HasSignature(buffer, 0, "RIFF", 4) &&
        HasSignature(buffer, 8, "WAVE", 4);
}

AudioFormat DetectFormat(const Core::ByteBuffer& buffer)
{
    if (IsOgg(buffer)) return AudioFormat::Ogg;
    if (IsWav(buffer)) return AudioFormat::Wav;

    return AudioFormat::Unknown;
}