#pragma once

enum class AudioFormat
{
    Unknown,
    Wav,
    Ogg
};

AudioFormat DetectFormat(const Core::ByteBuffer& buffer);