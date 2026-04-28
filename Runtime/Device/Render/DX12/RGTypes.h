#pragma once
#include <cstdint>

struct RGTexture
{
    uint32_t id = 0;
};

enum class RGAccess
{
    CopyDest,   // init / upload
    SRV,    // shader read
    UAV,    // unordered write/read
};

struct RGUsage
{
    RGTexture tex;
    RGAccess access;
};
