#pragma once
#include <cstdint>
#include "Core/Utils/Hash.h"

struct RGHandle
{
    uint32_t id{ 0 };

    bool operator==(const RGHandle& other) const = default;

    size_t GetHash() const
    {
        return Core::HashOf(id);
    }
};

struct RGHandleHasher
{
    size_t operator()(const RGHandle& handle) const noexcept 
    {
        return handle.GetHash();
    }
};

using PassIndex = int;
using RGResourceID = uint32_t;

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