#include "pch.h"
#include "Color.h"

namespace Core
{
    const Color Color::White{ 1.f, 1.f, 1.f, 1.f };
    const Color Color::Black{ 0.f, 0.f, 0.f, 1.f };
    const Color Color::Red{ 1.f, 0.f, 0.f, 1.f };
    const Color Color::Green{ 0.f, 1.f, 0.f, 1.f };
    const Color Color::Blue{ 0.f, 0.f, 1.f, 1.f };
    const Color Color::Yellow{ 1.f, 1.f, 0.f, 1.f };
    const Color Color::Cyan{ 0.f, 1.f, 1.f, 1.f };
    const Color Color::Magenta{ 1.f, 0.f, 1.f, 1.f };
    const Color Color::Gray{ 0.5f, 0.5f, 0.5f, 1.f };
    const Color Color::Transparent{ 0.f, 0.f, 0.f, 0.f };

    uint8_t Color::R8() const { return static_cast<uint8_t>(r * 255.f + 0.5f); } //버림이기 때문에 올림 처리 하기 위해서 0.5를 더한다.
    uint8_t Color::G8() const { return static_cast<uint8_t>(g * 255.f + 0.5f); }
    uint8_t Color::B8() const { return static_cast<uint8_t>(b * 255.f + 0.5f); }
    uint8_t Color::A8() const { return static_cast<uint8_t>(a * 255.f + 0.5f); }
}