#pragma once
#include "../Math/Vector4.h"

namespace Core
{
    struct Color
    {
        float r{ 1.f };
        float g{ 1.f };
        float b{ 1.f };
        float a{ 1.f };

        constexpr Color() = default;

        constexpr Color(float r, float g, float b, float a = 1.f)
            : r(r), g(g), b(b), a(a)
        {
        }

        constexpr explicit Color(const Vector4& v)
            : r(v.x), g(v.y), b(v.z), a(v.w)
        {
        }

        constexpr operator Vector4() const
        {
            return { r, g, b, a };
        }

        [[nodiscard]]
        constexpr Color WithAlpha(float alpha) const
        {
            return { r, g, b, alpha };
        }

        [[nodiscard]]
        static constexpr Color FromARGB8888(uint32_t argb)
        {
            return
            {
                ((argb >> 16) & 0xFF) / 255.f,
                ((argb >> 8) & 0xFF) / 255.f,
                (argb & 0xFF) / 255.f,
                ((argb >> 24) & 0xFF) / 255.f
            };
        }

        [[nodiscard]]
        static constexpr Color FromRGBA8888(uint32_t rgba)
        {
            return
            {
                ((rgba >> 24) & 0xFF) / 255.f,
                ((rgba >> 16) & 0xFF) / 255.f,
                ((rgba >> 8) & 0xFF) / 255.f,
                (rgba & 0xFF) / 255.f
            };
        }

        bool operator==(const Color&) const = default;
        bool operator!=(const Color&) const = default;

        uint8_t R8() const;
        uint8_t G8() const;
        uint8_t B8() const;
        uint8_t A8() const;

        static const Color White;
        static const Color Black;
        static const Color Red;
        static const Color Green;
        static const Color Blue;
        static const Color Yellow;
        static const Color Cyan;
        static const Color Magenta;
        static const Color Gray;
        static const Color Transparent;
    };

    static_assert(sizeof(Color) == sizeof(Vector4));
    static_assert(alignof(Color) == alignof(Vector4));
}

