#include "pch.h"
#include "WavStaticLoader.h"
#include "AudioProcessing.h"

#pragma warning(push)
#pragma warning(disable: 4244)
#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
#pragma warning(pop)

std::shared_ptr<StaticSoundAsset> WavStaticLoader::LoadFromMemory(Core::ByteBuffer buffer)
{
    drwav wav;

    if (!drwav_init_memory(&wav, buffer.data(), buffer.size(), nullptr))
        return nullptr;

    uint32_t channels = wav.channels;
    uint32_t sampleRate = wav.sampleRate;
    uint64_t totalFrames = wav.totalPCMFrameCount;

    if (channels == 0 || sampleRate == 0 || totalFrames == 0)
    {
        drwav_uninit(&wav);
        return nullptr;
    }

    std::vector<float> floatSamples;
    size_t sampleCount = static_cast<size_t>(totalFrames) * channels;
    floatSamples.resize(sampleCount);

    uint64_t framesRead = drwav_read_pcm_frames_f32(&wav, totalFrames, floatSamples.data());
    if (framesRead == 0)
    {
        drwav_uninit(&wav);
        return nullptr;
    }
    drwav_uninit(&wav);

    int inFrames = static_cast<int>(framesRead);
    if (inFrames <= 0) return nullptr;

    std::vector<float> processed;
    if (sampleRate != 48000)
        processed = ResampleCubic(floatSamples.data(), inFrames, channels, sampleRate, 48000);
    else
        processed = std::move(floatSamples);

    std::vector<uint8_t> finalData;
    ConvertFloatToInt16(processed.data(), static_cast<int>(processed.size()), finalData);

    auto asset = std::make_shared<StaticSoundAsset>();
    asset->channels = channels;
    asset->sampleRate = 48000;
    asset->format = SampleFormat::Int16;
    asset->data = std::move(finalData);

    return asset;
}

///////////////////////////////////////////////////////

unique_ptr<IAssetLoader> CreateWavStaticLoader()
{
    return make_unique<WavStaticLoader>();
}