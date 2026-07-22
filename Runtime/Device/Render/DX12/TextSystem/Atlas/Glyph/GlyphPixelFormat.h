#pragma once
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

inline DXGI_FORMAT GetDXGIFormat(GlyphPixelFormat format)
{
    switch (format)
    {
    case GlyphPixelFormat::R8: return DXGI_FORMAT_R8_UNORM;
    case GlyphPixelFormat::RG8: return DXGI_FORMAT_R8G8_UNORM;
    case GlyphPixelFormat::RGBA8: return DXGI_FORMAT_R8G8B8A8_UNORM;
    }

    Assert(false);
    return DXGI_FORMAT_UNKNOWN;
}
