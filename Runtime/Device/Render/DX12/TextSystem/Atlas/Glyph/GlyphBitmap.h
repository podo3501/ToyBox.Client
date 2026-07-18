#pragma once
#include <vector>
#include <cstdint>

struct GlyphBitmap
{
    std::vector<uint8_t> pixels;

    uint32_t width{ 0 };
    uint32_t height{ 0 };

    float glyphWidth{ 0.0f }; // 실제 glyph metric
    float glyphHeight{ 0.0f };

    float bearingX{ 0.0f };
    float bearingY{ 0.0f };

    uint32_t channels{ 1 };
    float advanceX{ 0.0f };
};