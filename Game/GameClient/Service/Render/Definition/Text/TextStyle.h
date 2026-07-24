#pragma once
#include "Core/Foundation/Color.h"

enum class TextRenderMode
{
    MTSDF, //일반적인 크기 및 전반적으로 사용.
    Bitmap, //크기가 작은 폰트에서 제한적으로 사용.
    Count
};

enum class OutlineWeight
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
};

enum class OutlineColor
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

struct TextStyle
{
    TextRenderMode mode{ TextRenderMode::MTSDF };
    Core::Color color{ Core::Color::White };
    OutlineWeight weight{ OutlineWeight::None };
    OutlineColor outlineColor{ OutlineColor::Black };
};