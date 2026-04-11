#include "pch.h"
#include "AudioProcessing.h"


static float CubicHermite(float prev, float curr, float next, float next2, float t)
{
    float a = (-prev + 3.0f * curr - 3.0f * next + next2) * 0.5f;
    float b = (2.0f * prev - 5.0f * curr + 4.0f * next - next2) * 0.5f;
    float c = (-prev + next) * 0.5f;
    float d = curr;

    return ((a * t + b) * t + c) * t + d;
}

std::vector<float> ResampleCubic(const float* input, int inFrames, int channels, int inRate, int outRate)
{
    double step = (double)inRate / outRate;
    int outFrames = static_cast<int>(std::ceil(inFrames / step));

    std::vector<float> output(outFrames * channels);

    for (int i = 0; i < outFrames; ++i)
    {
        double srcPos = i * step;
        int idx = (int)srcPos;
        float t = (float)(srcPos - idx);

        for (int ch = 0; ch < channels; ++ch)
        {
            auto sample = [&](int frame) {
                if (frame < 0)
                    frame = 0;
                else if (frame >= inFrames)
                    frame = inFrames - 1;

                return input[frame * channels + ch];
                };

            float p0 = sample(idx - 1);
            float p1 = sample(idx);
            float p2 = sample(idx + 1);
            float p3 = sample(idx + 2);

            output[i * channels + ch] =
                CubicHermite(p0, p1, p2, p3, t);
        }
    }

    return output;
}

void ConvertFloatToInt16(const float* input, int sampleCount, std::vector<uint8_t>& outData)
{
    outData.resize(sampleCount * sizeof(int16_t));

    int16_t* out = reinterpret_cast<int16_t*>(outData.data());
    for (int i = 0; i < sampleCount; ++i)
    {
        float v = std::clamp(input[i], -1.0f, 1.0f);
        out[i] = static_cast<int16_t>(v * 32767.0f);
    }
}