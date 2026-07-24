#pragma once
#include "GlyphPixelFormat.h"
#include <vector>

struct GlyphPixels
{
    GlyphPixelFormat format{ GlyphPixelFormat::R8 };
    std::vector<uint8_t> buffer;

    uint32_t width{ 0 }; //atlas에 들어가는 크기 정보. 즉 uv 정보가 된다. 
    uint32_t height{ 0 };
};

struct GlyphMetrics
{
    float width{ 0.f }; //bitmap font는 uv와 이미지의 크기가 동일 하기 때문에 이미지 크기로도 쓰인다.
    float height{ 0.f };
    float pxRange{ 0.f };

    float bearingX{ 0.f };
    float bearingY{ 0.f };
    float advanceX{ 0.f };
};

struct BitmapGlyph
{
    GlyphPixels pixels;
    GlyphMetrics metrics;
};

struct MTSDFGlyph
{
    GlyphPixels pixels;
    GlyphMetrics metrics;
};