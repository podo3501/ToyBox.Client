#pragma once

namespace Core
{
    // 4비트 고정 니블(nibble) 단위로 값을 순서대로 패킹. 최대 8개까지 (4 * 8 = 32비트)
    template <typename... Ts>
    constexpr uint32_t PackNibbles(Ts... values)
    {
        static_assert(sizeof...(Ts) <= 8, "니블 8개(32비트)를 초과할 수 없습니다.");
        assert(((static_cast<uint32_t>(values) < 16) && ...)); //값이 16을 넘어가면 짤린다.

        uint32_t packed = 0;
        uint32_t offset = 0;
        ((packed |= (static_cast<uint32_t>(values) & 0x0F) << offset, offset += 4), ...); // fold expression으로 순서대로 4비트씩 채워넣기
        return packed;
    }

    template <typename... Ts>
    constexpr std::tuple<Ts...> UnpackNibbles(uint32_t packed, uint32_t startIndex = 0)
    {
        static_assert(sizeof...(Ts) <= 8, "니블 8개(32비트)를 초과할 수 없습니다.");

        uint32_t offset = startIndex * 4;
        return std::tuple<Ts...>{
            [&]() -> Ts
                {
                    Ts value = static_cast<Ts>((packed >> offset) & 0x0F);
                    offset += 4;
                    return value;
                }()...
        };
    }
}