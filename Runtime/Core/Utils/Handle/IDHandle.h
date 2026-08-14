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

template<typename ToTag, typename FromTag>
constexpr IDHandle<ToTag> HandleCast(IDHandle<FromTag> h) noexcept
{
    return IDHandle<ToTag>{ h.index, h.generation };
}

namespace std //unordered_map에서 hash를 어떻게 계산할지 정해주어야 한다.
{
    template<typename Tag>
    struct hash<IDHandle<Tag>>
    {
        size_t operator()(const IDHandle<Tag>& h) const noexcept
        {
            size_t seed = h.index;
            seed ^= (h.generation + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2));
            return seed;
        }
    };
}

/*
    사용법: Name에 이름 바꿔 넣기.
    struct NameTag {};
    using NameHandle = IDHandle<NameTag>;
    inline constexpr NameHandle InvalidNameHandle = IDHandle<NameTag>::Invalid();
*/
