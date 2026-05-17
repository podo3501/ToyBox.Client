#pragma once

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
};

struct ResourceKeyHash
{
    size_t operator()(const ResourceKey& k) const
    {
        return std::hash<std::string>{}(k.id) ^ (size_t)k.type;
    }
};