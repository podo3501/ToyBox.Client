#pragma once

std::vector<float> ResampleCubic(const float* input, int inFrames, int channels, int inRate, int outRate);
void ConvertFloatToInt16(const float* input, int sampleCount, std::vector<uint8_t>& outData);
