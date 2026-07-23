#pragma once
#include "Core/Foundation/Color.h"

enum class TextRenderMode
{
    SDF, //일반적인 크기 및 전반적으로 사용.
    MTSDF, //크기가 큰 폰트에 사용.
    Bitmap, //크기가 작은 폰트에서 제한적으로 사용.
    Count
};

struct TextStyle
{
    TextRenderMode mode{ TextRenderMode::Bitmap }; //?!? 나중에는 SDF가 기본이 된다.
    Core::Color color{ Core::Color::White };
};