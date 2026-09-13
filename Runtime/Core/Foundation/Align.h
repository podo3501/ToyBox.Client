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

    // AlignUp과 달리 alignment가 2의 거듭제곱이 아니어도 동작하는 범용 버전.
    // (예: sizeof(UIVertex)=72처럼 임의의 stride로 정렬해야 하는 경우)
    // 2의 거듭제곱일 때는 비트 연산으로, 아닐 때는 나눗셈으로 처리한다.
    // 정수 전용 (포인터는 stride 정렬 대상이 아니므로 지원하지 않음).
    template <typename T>
        requires std::integral<T> && (!std::same_as<T, bool>)
    [[nodiscard]] constexpr T AlignUpGeneric(T value, T alignment) noexcept
    {
        Assert(alignment > 0);

        if (std::has_single_bit(static_cast<size_t>(alignment)))
            return AlignUp(value, static_cast<size_t>(alignment));

        return ((value + alignment - 1) / alignment) * alignment;
    }
}