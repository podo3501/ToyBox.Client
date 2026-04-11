#pragma once
#include "ogg/ogg.h"
#include "vorbis/vorbisfile.h"

class VorbisDecoder
{
public:
    VorbisDecoder() = default;
    ~VorbisDecoder();

    bool Open(const void* data, size_t size);
    std::vector<float> DecodeAll(int& outFrames, int& outChannels);
    void Close();
    bool IsOpened() const;
    const vorbis_info* GetInfo() const;

private:
    //callback functions
    static size_t ReadCB(void* ptr, size_t size, size_t nmemb, void* datasource);
    static int SeekCB(void* datasource, ogg_int64_t offset, int whence);
    static long TellCB(void* datasource);
    static int CloseCB(void* datasource);

private:
    OggVorbis_File vf{};
    ov_callbacks cb{};
    bool opened = false;

    struct MemoryContext
    {
        const unsigned char* data = nullptr;
        size_t size = 0;
        size_t offset = 0;
    } ctx;
};
