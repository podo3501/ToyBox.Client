#pragma once

struct ImageData
{
    std::vector<uint8_t> pixels;
    uint32_t width;
    uint32_t height;
    uint32_t stride;
};
