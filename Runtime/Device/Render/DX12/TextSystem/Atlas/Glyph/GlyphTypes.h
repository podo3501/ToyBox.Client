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
    // 렌더링 quad 크기(px). BitmapGlyph는 pixels.width/height와 항상 동일하지만,
    // MTSDFGlyph는 CreateGlyphInfo()에서 scale이 적용되어 pixels 크기와 달라질 수 있음.
    float width{ 0.f };
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