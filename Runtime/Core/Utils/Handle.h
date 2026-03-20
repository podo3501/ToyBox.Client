#pragma once
#include <cstdint>
#include <functional>

template<typename Tag>
struct Handle
{
    std::uint32_t value;
    constexpr explicit Handle(std::uint32_t v) : value(v) {}
    auto operator<=>(const Handle&) const = default;
};

namespace std
{
    template<typename Tag>
    struct hash<Handle<Tag>> //std 네임스페이스에 hash 템플릿 특수화를 이용해서 기능을 넣는다. std에 기능을 넣는건 hash만 추천. 다른 std에는 넣거나 변경은 추천하지 않음.
    {
        size_t operator()(const Handle<Tag>& h) const noexcept
        {
            return hash<uint32_t>{}(h.value);
        }
    };
}

template<typename Tag>
inline constexpr Handle<Tag> InvalidHandle{ 0 }; //핸들은 0값을 오류 값으로 사용한다.

/*
    사용법: Name에 이름 바꿔 넣기.
    struct NameTag {};
    using NameHandle = Handle<NameTag>;
    inline constexpr NameHandle InvalidNameHandle = InvalidHandle<NameTag>;
*/
