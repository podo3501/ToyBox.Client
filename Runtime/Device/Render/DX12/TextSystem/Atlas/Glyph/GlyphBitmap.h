#pragma once
#include <vector>
#include <cstdint>

enum class GlyphPixelFormat
{
    R8,
    RG8,
    RGBA8,
};

inline uint32_t GetBytesPerPixel(GlyphPixelFormat format)
{
    switch (format)
    {
    case GlyphPixelFormat::R8: return 1;
    case GlyphPixelFormat::RG8: return 2;
    case GlyphPixelFormat::RGBA8: return 4;
    }

    Assert(false);
    return 1;
}

struct GlyphBitmap
{
    GlyphPixelFormat format{ GlyphPixelFormat::R8 };
    std::vector<uint8_t> pixels;

    uint32_t width{ 0 };
    uint32_t height{ 0 };

    float bearingX{ 0.0f };
    float bearingY{ 0.0f };
    float advanceX{ 0.0f };
};

struct SDFGlyphBitmap
{
    GlyphBitmap bitmap;

    float glyphWidth{ 0.0f }; // 실제 glyph metric
    float glyphHeight{ 0.0f };
};