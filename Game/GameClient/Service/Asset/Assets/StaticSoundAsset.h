#pragma once
#include "GameCore/Service/Asset/Asset.h"
#include <vector>

enum class SampleFormat
{
    Int16,
    Float32
};

struct StaticSoundAsset : public Asset
{
    CORE_DECLARE_TYPE(StaticSoundAsset)

    int sampleRate = 0;
    int channels = 0;
    SampleFormat format = SampleFormat::Int16;

    std::vector<uint8_t> data;

    template<typename T>
    T* As()
    {
        return reinterpret_cast<T*>(data.data());
    }

    template<typename T>
    const T* As() const
    {
        return reinterpret_cast<const T*>(data.data());
    }

    size_t SampleCount() const
    {
        size_t bytesPerSample = (format == SampleFormat::Int16) ? sizeof(int16_t) : sizeof(float);
        return data.size() / bytesPerSample;
    }
};