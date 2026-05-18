#pragma once
#include <functional>

namespace Core
{
    template<typename T>
    inline void HashCombine(size_t& seed, const T& value)
    {
        seed ^= std::hash<T>{}(value)
            +0x9e3779b9
            + (seed << 6)
            + (seed >> 2);
    }

    template<typename... Args>
    inline size_t HashOf(const Args&... args)
    {
        size_t seed = 0;

        (HashCombine(seed, args), ...);

        return seed;
    }
}