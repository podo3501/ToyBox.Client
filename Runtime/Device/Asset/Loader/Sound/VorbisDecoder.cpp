#include "pch.h"
#include "VorbisDecoder.h"

VorbisDecoder::~VorbisDecoder()
{
    Close();
}

bool VorbisDecoder::Open(const void* data, size_t size)
{
    Close();

    ctx.data = static_cast<const unsigned char*>(data);
    ctx.size = size;
    ctx.offset = 0;

    cb.read_func = ReadCB;
    cb.seek_func = SeekCB;
    cb.tell_func = TellCB;
    cb.close_func = CloseCB;

    if (ov_open_callbacks(&ctx, &vf, nullptr, 0, cb) < 0)
        return false;

    opened = true;
    return true;
}

void VorbisDecoder::Close()
{
    if (opened)
    {
        ov_clear(&vf);
        opened = false;
    }
}

bool VorbisDecoder::IsOpened() const
{
    return opened;
}

const vorbis_info* VorbisDecoder::GetInfo() const
{
    if (!opened) return nullptr;
    return ov_info(const_cast<OggVorbis_File*>(&vf), -1);
}

std::vector<float> VorbisDecoder::DecodeAll(int& outFrames, int& outChannels)
{
    outFrames = 0;
    outChannels = 0;

    const vorbis_info* info = GetInfo();
    if (!info) return {};

    outChannels = info->channels;

    std::vector<float> temp;
    int currentSection = 0;

    while (true)
    {
        float** pcm = nullptr;

        long samples = ov_read_float(
            &vf,
            &pcm,
            1024,
            &currentSection);

        if (samples <= 0)
            break;

        size_t oldSize = temp.size();
        size_t sampleCount = static_cast<size_t>(samples) * outChannels;

        temp.resize(oldSize + sampleCount);

        for (int ch = 0; ch < outChannels; ch++)
        {
            for (int i = 0; i < samples; i++)
            {
                temp[oldSize + i * outChannels + ch] = pcm[ch][i];
            }
        }

        outFrames += static_cast<int>(samples);
    }

    return temp;
}

size_t VorbisDecoder::ReadCB(void* ptr, size_t size, size_t nmemb, void* datasource)
{
    if (size == 0) return 0;

    auto* ctx = static_cast<MemoryContext*>(datasource);

    size_t bytesRequested = size * nmemb;
    size_t remaining = ctx->size - ctx->offset;

    size_t bytesToRead = (bytesRequested < remaining) ? bytesRequested : remaining;
    if (bytesToRead == 0) return 0;

    memcpy(ptr, ctx->data + ctx->offset, bytesToRead);
    ctx->offset += bytesToRead;

    return bytesToRead / size;
}

int VorbisDecoder::SeekCB(void* datasource, ogg_int64_t offset, int whence)
{
    auto* ctx = static_cast<MemoryContext*>(datasource);

    int64_t pos = 0;
    switch (whence)
    {
    case SEEK_SET: pos = offset; break;
    case SEEK_CUR: pos = (int64_t)ctx->offset + offset; break;
    case SEEK_END: pos = (int64_t)ctx->size + offset; break;
    default: return -1;
    }
    if (pos < 0 || pos >(int64_t)ctx->size) return -1;

    ctx->offset = static_cast<size_t>(pos);
    return 0;
}

long VorbisDecoder::TellCB(void* datasource)
{
    auto* ctx = static_cast<MemoryContext*>(datasource);
    return static_cast<long>(ctx->offset);
}

int VorbisDecoder::CloseCB(void*)
{
    return 0;
}