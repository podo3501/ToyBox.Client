#pragma once
#include <cstdint>

struct RGTexture
{
    uint32_t id = 0;
};

enum class RGAccess
{
    Read,   // ¿œπ› read
    SRV,    // shader read
    UAV,    // unordered write/read
    Write   // init / upload
};

struct RGUsage
{
    RGTexture tex;
    RGAccess access;
};
