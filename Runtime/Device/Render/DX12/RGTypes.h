#pragma once
#include <cstdint>

struct RGResource
{
    uint32_t id{ 0 };
};

enum class RGAccess
{
    CopyDest,   // init / upload
    SRV,    // shader read
    UAV,    // unordered write/read
};

struct RGUsage
{
    RGResource resource;
    RGAccess access;
};