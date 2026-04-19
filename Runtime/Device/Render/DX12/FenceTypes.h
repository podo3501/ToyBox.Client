#pragma once
#include <cstdint>
#include <algorithm>

struct QueueFences
{
    uint64_t direct;
    uint64_t copy;

    void Merge(const QueueFences& other);
};

inline void QueueFences::Merge(const QueueFences& other)
{
    direct = std::max(direct, other.direct);
    copy = std::max(copy, other.copy);
}