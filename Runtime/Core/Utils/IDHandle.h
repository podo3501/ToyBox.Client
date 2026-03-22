#pragma once
#include <cstdint>
#include <functional>

//ID형 핸들은 핸들값이 id의 사용법과 같을때이다. value가 증가만하고 generation이 없을때 사용.
//index가 증가만 하기 때문에 vector처럼 정해진 크기에는 사용하지 못한다. 대신 32비트를 전부를 사용한다.
//정해진 크기가 있다면 vector를 써서 빠르게 참조 가능한데, 이때는 슬롯을 재사용하기 때문에 generation을 이용해서 실제 handle 주인을 찾아야 한다.

template<typename Tag>
struct IDHandle
{
    uint32_t value{ 0 };
    constexpr IDHandle() = default;
    constexpr explicit IDHandle(uint32_t v) : value(v) {}
    auto operator<=>(const IDHandle&) const = default;

    constexpr explicit operator bool() const noexcept { return value != 0; }
    static constexpr IDHandle Invalid() noexcept { return IDHandle{ 0 }; }
};

namespace std
{
    template<typename Tag>
    struct hash<IDHandle<Tag>> //std 네임스페이스에 hash 템플릿 특수화를 이용해서 기능을 넣는다. std에 기능을 넣는건 hash만 추천. 다른 std에는 넣거나 변경은 추천하지 않음.
    {
        size_t operator()(const IDHandle<Tag>& h) const noexcept
        {
            return hash<uint32_t>{}(h.value);
        }
    };
}

template<typename Tag>
inline constexpr IDHandle<Tag> InvalidIDHandle{ 0 }; //핸들은 0값을 오류 값으로 사용한다.

/*
    사용법: Name에 이름 바꿔 넣기.
    struct NameTag {};
    using NameHandle = IDHandle<NameTag>;
    inline constexpr NameHandle InvalidNameHandle = IDHandle<NameTag>::Invalid();
*/
