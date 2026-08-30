#pragma once
#include <cstdint>
#include "Core/Utils/Hash.h"

using PassIndex = int;
using RGResourceID = uint32_t;
static constexpr RGResourceID InvalidRGID = std::numeric_limits<RGResourceID>::max();

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