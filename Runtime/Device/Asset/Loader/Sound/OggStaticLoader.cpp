#include "pch.h"
#include "OggStaticLoader.h"
#include "GameClient/Service/Asset/Assets/StaticSoundAsset.h"
#include "AudioProcessing.h"
#include "VorbisDecoder.h"

std::shared_ptr<Asset> OggStaticLoader::LoadFromMemory(const Core::ByteBuffer& buffer)
{
    VorbisDecoder decoder;

    if (!decoder.Open(buffer.data(), buffer.size()))
        return nullptr;

    const vorbis_info* info = decoder.GetInfo();
    if (!info)
        return nullptr;

    const int srcRate = info->rate;
    const int dstRate = 48000;

    if (info->channels <= 0 || srcRate <= 0)
        return nullptr;

    int totalFrames = 0;
    int channels = 0;

    std::vector<float> pcm = decoder.DecodeAll(totalFrames, channels);

    if (pcm.empty() || totalFrames <= 0)
        return nullptr;

    std::vector<float> finalPCM;
    if (srcRate != dstRate)
        finalPCM = ResampleCubic(pcm.data(), totalFrames, channels, srcRate, dstRate);
    else
        finalPCM = std::move(pcm);

    std::vector<uint8_t> pcm16;
    ConvertFloatToInt16(finalPCM.data(), static_cast<int>(finalPCM.size()), pcm16);

    auto asset = std::make_shared<StaticSoundAsset>();
    asset->channels = channels;
    asset->sampleRate = dstRate;
    asset->format = SampleFormat::Int16;
    asset->data = std::move(pcm16);

    return asset;
}

///////////////////////////////////////////////////////

unique_ptr<IAssetLoader> CreateOggStaticLoader()
{
	return make_unique<OggStaticLoader>();
}