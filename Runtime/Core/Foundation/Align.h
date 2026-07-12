#pragma once
#include <bit>
#include <concepts>
#include "Assert.h"

namespace Core
{
    template <typename T>
    concept Alignable = (std::integral<T> && !std::same_as<T, bool>) || std::is_pointer_v<T>;

    template <Alignable T>
    [[nodiscard]] constexpr T AlignUp(T value, size_t alignment) noexcept
    {
        Assert(std::has_single_bit(alignment));

        // 포인터든 정수든 일단 64비트 정수(uintptr_t)로 통일해서 연산합니다.
        uintptr_t rawValue;
        if constexpr (std::is_pointer_v<T>)
            rawValue = reinterpret_cast<uintptr_t>(value);
        else
            rawValue = static_cast<uintptr_t>(value);

        uintptr_t aligned = (rawValue + alignment - 1) & ~(alignment - 1);

        // 다시 원래 타입 T로 안전하게 복원해서 반환
        if constexpr (std::is_pointer_v<T>)
            return reinterpret_cast<T>(aligned);
        else
            return static_cast<T>(aligned);
    }

    template <Alignable T>
    [[nodiscard]] constexpr T AlignDown(T value, size_t alignment) noexcept
    {
        assert(std::has_single_bit(alignment));

        uintptr_t rawValue;
        if constexpr (std::is_pointer_v<T>)
            rawValue = reinterpret_cast<uintptr_t>(value);
        else
            rawValue = static_cast<uintptr_t>(value);

        uintptr_t aligned = rawValue & ~(alignment - 1);

        if constexpr (std::is_pointer_v<T>)
            return reinterpret_cast<T>(aligned);
        else
            return static_cast<T>(aligned);
    }
}