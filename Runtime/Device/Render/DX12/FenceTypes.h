#pragma once
#include <cstdint>

struct QueueFences
{
    uint64_t direct;
    uint64_t copy;
};