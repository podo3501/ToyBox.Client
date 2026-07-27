#pragma once
#include "Core/Foundation/Color.h"

enum class TextRenderMode
{
    MTSDF, //일반적인 크기 및 전반적으로 사용.
    Bitmap, //크기가 작은 폰트에서 제한적으로 사용.
    Count
};

//text style은 4비트로 패킹되기 때문에 즉, enum 갯수가 16개를 넘기면 안된다. 

enum class TextEffectLevel
{
    None = 0,
    Level1,
    Level2,
    Level3,
    Level4,
    Level5,
    Level6,
    Level7,
    Level8,
    Level9,
    Level10,
    Level11,
    Level12,
};

enum class TextColor
{
    Black = 0,
    DarkGray,
    LightGray,
    White,
    Navy,
    Charcoal,
    DarkBrown,
    Gold
};

struct TextOutline
{
    TextEffectLevel weight{ TextEffectLevel::None };
    TextColor color{ TextColor::Black };

    constexpr auto ToNibbles() const
    {
        return std::make_tuple(weight, color);
    }
};

struct TextShadow
{
    TextEffectLevel offset{ TextEffectLevel::None };
    TextEffectLevel softness{ TextEffectLevel::None };
    TextColor color{ TextColor::Black };

    constexpr auto ToNibbles() const
    {
        return std::make_tuple(offset, softness, color);
    }
};

struct TextGradient
{
    TextColor startColor{ TextColor::Black };
    TextColor endColor{ TextColor::Black };

    constexpr auto ToNibbles() const
    {
        return std::make_tuple(startColor, endColor);
    }
};

struct TextGlow
{
    TextEffectLevel range{ TextEffectLevel::None };
    TextEffectLevel intensity{ TextEffectLevel::None };
    TextColor color{ TextColor::Black };

    constexpr auto ToNibbles() const
    {
        return std::make_tuple(range, intensity, color);
    }
};

struct TextStyle
{
    TextRenderMode mode{ TextRenderMode::MTSDF };
    Core::Color color{ Core::Color::White };

    TextOutline outline;
    TextShadow shadow;
    TextGradient gradient;
    TextGlow glow;
};