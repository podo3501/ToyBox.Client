#include "pch.h"
#include "StreamSoundBuffer.h"
#include "Platform/Resource/IResourceStream.h"
#include "Audio/AudioTypes.h"
#include "../Srcs/SDL-mixer-3.1.0/src/decoder_stb_vorbis.c"

StreamSoundBuffer::~StreamSoundBuffer() = default;
StreamSoundBuffer::StreamSoundBuffer() = default;

bool StreamSoundBuffer::Load(unique_ptr<IResourceStream> stream, AudioGroupID groupID, float volume)
{
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