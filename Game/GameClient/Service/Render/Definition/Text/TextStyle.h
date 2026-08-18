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
    Level13,
    Level14,
    Level15,
};

enum class TextColor
{
    Black = 0,
    DarkGray,
    Gray,
    LightGray,
    White,

    Red,
    Green,
    Blue,
    Yellow,

    Orange,
    Purple,
    Cyan,

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

struct TextUnderline
{
    std::optional<Core::Color> color; // 미지정 시 TextStyle::color를 사용
    float thickness{ -1.f }; // -1이면 폰트 메트릭에서 자동 산출
};

enum class TextHorizontalAlign
{
    Left, 
    Center, 
    Right
};

enum class TextVerticalAlign
{
    Top,
    Middle,
    Bottom
};

enum class TextOverflow
{
    Overflow, // 영역 밖으로 그림
    Clip, // Rect 밖은 자름.
};

struct TextLayout
{
    TextHorizontalAlign horizontalAlign{ TextHorizontalAlign::Left };
    TextVerticalAlign verticalAlign{ TextVerticalAlign::Top };

    bool wordWrap{ true };
    TextOverflow overflow{ TextOverflow::Clip };

    float lineSpacing{ 1.f }; //폰트 lineHeight의 배율. 1.0 이면 lineHeight를 그대로 쓴다는 것.
};

struct TextStyle
{
    Core::Color color{ Core::Color::White };

    std::optional<TextOutline> outline;
    std::optional<TextShadow> shadow;
    std::optional<TextGradient> gradient;
    std::optional<TextGlow> glow;
    std::optional<TextUnderline> underline;
};

struct TextRun
{
    std::vector<char32_t> codePoints;
    TextStyle style;
    uint32_t lineIndex{ 0 }; // 이 run이 몇 번째 줄에 속하는지 (절대값)
};

struct TextSpan
{
    std::string_view text;
    TextStyle style;
};