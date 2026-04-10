#include "pch.h"
#include "OggSoundLoader.h"
#include "GameClient/Service/Asset/Assets/SoundAsset.h"
//#define STB_VORBIS_IMPLEMENTATION
#include "stb_vorbis.c"

static void ConvertFloatToInt16(const float* input, int sampleCount, std::vector<uint8_t>& outData)
{
    outData.resize(sampleCount * sizeof(int16_t));

    int16_t* out = reinterpret_cast<int16_t*>(outData.data());
    for (int i = 0; i < sampleCount; ++i)
    {
        float v = std::clamp(input[i], -1.0f, 1.0f);
        out[i] = static_cast<int16_t>(v * 32767.0f);
    }
}

std::shared_ptr<Asset> OggSoundLoader::Load(const Core::ByteBuffer& buffer)
{
    // 버퍼 유효성 검사
    if (buffer.empty())
    {
        printf("Error: Empty buffer provided to OggSoundLoader\n");
        return nullptr;
    }


    // stb_vorbis 에러 코드
    int error = 0;
    const unsigned char* rawBuffer = reinterpret_cast<const unsigned char*>(buffer.data());

    // Ogg 매직 넘버 확인 (빠른 검증)
    if (buffer.size() < 4 ||
        rawBuffer[0] != 'O' || rawBuffer[1] != 'g' ||
        rawBuffer[2] != 'g' || rawBuffer[3] != 'S')
    {
        printf("Error: Invalid Ogg file format (missing 'OggS' magic number)\n");
        return nullptr;
    }

    // stb_vorbis 스트림 열기
    stb_vorbis* vorbis = stb_vorbis_open_memory(
        rawBuffer,
        static_cast<int>(buffer.size()),
        &error,
        nullptr
    );

    if (!vorbis)
    {
        printf("Error: Failed to open Ogg Vorbis stream (error code: %d)\n", error);
        //PrintDebugInfo(rawBuffer, buffer.size());
        return nullptr;
    }

    return nullptr;









    //short* output = nullptr; // 디코딩된 PCM 데이터가 담길 포인터
    //int channels = 0;
    //int sampleRate = 0;

    //// 한 번의 호출로 전체 데이터를 short(Int16) 배열로 변환
    //int totalSamplesPerChannel = stb_vorbis_decode_memory(
    //    reinterpret_cast<const unsigned char*>(buffer.data()),
    //    static_cast<int>(buffer.size()),
    //    &channels,
    //    &sampleRate,
    //    &output
    //);

    //// 에러 체크: 샘플 수가 0보다 작으면 디코딩 실패
    //if (totalSamplesPerChannel < 0)
    //{
    //    // 여기서도 에러가 난다면 파일 헤더 자체가 stb_vorbis와 호환되지 않는 것임
    //    return nullptr;
    //}

    //auto asset = std::make_shared<SoundAsset>();
    //asset->sampleRate = sampleRate;
    //asset->channels = channels;
    //asset->format = SampleFormat::Int16;

    //size_t totalSamples = static_cast<size_t>(totalSamplesPerChannel) * channels;
    //size_t byteSize = totalSamples * sizeof(short);

    //asset->data.resize(byteSize);
    //std::memcpy(asset->data.data(), output, byteSize);

    //free(output);

    //return asset;






    //int error = 0;
    //auto rawBuffer = (const unsigned char*)buffer.data();
    //stb_vorbis* vorbis = stb_vorbis_open_memory(rawBuffer,
    //    static_cast<int>(buffer.size()), &error, nullptr);
    //if (!vorbis) 
    //{
    //    printf("stb_vorbis error: %d\n", error);

    //    // 앞 32바이트 정도 hex dump 찍어보기
    //    printf("First 32 bytes: ");
    //    for (int i = 0; i < std::min(32, (int)buffer.size()); i++) {
    //        printf("%02X ", (unsigned char)buffer[i]);
    //    }
    //    printf("\n");

    //    // "OggS" 있는지 확인
    //    if (buffer.size() >= 4 &&
    //        rawBuffer[0] == 'O' && rawBuffer[1] == 'g' && rawBuffer[2] == 'g' && rawBuffer[3] == 'S') {
    //        printf("Capture pattern 'OggS' is present!\n");
    //    }
    //    else {
    //        printf("Capture pattern MISSING!\n");
    //    }

    //    return nullptr;
    //}

    //if (!vorbis) 
    //    return nullptr;

    //stb_vorbis_info info = stb_vorbis_get_info(vorbis);

    //auto asset = std::make_shared<SoundAsset>();
    //asset->sampleRate = info.sample_rate;
    //asset->channels = info.channels;
    //asset->format = SampleFormat::Int16;

    //int totalSamples = stb_vorbis_stream_length_in_samples(vorbis);

    //std::vector<float> temp;
    //temp.resize(totalSamples * asset->channels);

    //int samplesRead = stb_vorbis_get_samples_float_interleaved(
    //    vorbis,
    //    asset->channels,
    //    temp.data(),
    //    (int)temp.size()
    //);

    //int totalRead = samplesRead * asset->channels;

    //ConvertFloatToInt16(
    //    temp.data(),
    //    totalRead,
    //    asset->data
    //);

    //stb_vorbis_close(vorbis);

    //return asset;
}

///////////////////////////////////////////////////////

unique_ptr<IAssetLoader> CreateOggSoundLoader()
{
	return make_unique<OggSoundLoader>();
}