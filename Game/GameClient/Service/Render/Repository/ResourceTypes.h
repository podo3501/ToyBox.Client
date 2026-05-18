#pragma once
#include "Core/Utils/Hash.h"
#include "Core/Utils/EnumHelpers.h"

enum class LoadState
{
    Pending,
    CpuLoading,
    GpuLoading,
    Ready,
    Failed
};

struct ResourceKey
{
    enum class Type
    {
        File,
        Runtime
    };

    std::string id;
    Type type;

    bool operator==(const ResourceKey& other) const
    {
        return type == other.type && id == other.id;
    }

    size_t GetHash() const
    {
        return Core::HashOf(id, type);
    }
};

struct ResourceKeyHash
{
    size_t operator()(const ResourceKey& k) const
    {
        return k.GetHash();
    }
};