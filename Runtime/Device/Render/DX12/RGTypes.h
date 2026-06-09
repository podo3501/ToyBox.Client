#pragma once
#include <cstdint>

struct RGHandle
{
    uint32_t id{ 0 };
};

enum class RGAccess
{
    CopyDest,   // init / upload
    SRV,    // shader read
    UAV,    // unordered write/read
    RTV,    // backbuffer

    DepthWrite,
    DepthRead,

    Present, // present
};

struct RGUsage
{
    RGHandle handle;
    RGAccess access;
};