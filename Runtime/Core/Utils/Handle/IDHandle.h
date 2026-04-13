#pragma once
#include <cstdint>
#include <functional>

template<typename Tag>
struct IDHandle
{
    uint32_t index{ 0 };
    uint32_t generation{ 0 };

    constexpr IDHandle() = default;
    constexpr IDHandle(uint32_t i, uint32_t g) : index(i), generation(g) {}

    auto operator<=>(const IDHandle&) const = default;

    constexpr explicit operator bool() const noexcept { return index != 0; } // 0은 항상 invalid
    static constexpr IDHandle Invalid() noexcept { return IDHandle{ 0, 0 }; }
};

/*
    사용법: Name에 이름 바꿔 넣기.
    struct NameTag {};
    using NameHandle = IDHandle<NameTag>;
    inline constexpr NameHandle InvalidNameHandle = IDHandle<NameTag>::Invalid();
*/
