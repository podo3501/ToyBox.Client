#pragma once
#include "Core/Math/Vector2.h"

struct GlyphInfo
{
    float width{ 0.0f }; //글리프 비트맵의 크기
    float height{ 0.0f };
    float bearingX{ 0.0f }; //pen 위치에서 비트맵 왼쪽까지의 거리. pen은 현재 쓰여질 위치.
    float bearingY{ 0.0f }; //pen 위치에서 위쪽까지의 거리
    float advanceX{ 0.0f }; //다음 글자를 그리기 위해 pen을 얼마나 이동할지. 소수점 아래 6비트를 사용하는 고정소수점. 즉 26.6 고정소수점 변환
    Core::Math::Vector2 uvMin{ 0.0f, 0.0f }; //Atlas Texture의 좌상단 UV. 즉 source
    Core::Math::Vector2 uvMax{ 0.0f, 0.0f }; //Atlas Texture의 우하단 UV
};