#pragma once
#include "Core/Math/Vector2.h"

struct GlyphInfo
{
    float width{ 0.0f }; //글리프 비트맵의 크기
    float height{ 0.0f };
    float bearingX{ 0.0f }; //pen 위치에서 비트맵 왼쪽까지의 거리. pen은 현재 쓰여질 위치.
    float bearingY{ 0.0f }; //pen 위치에서 위쪽까지의 거리
    
    Core::Vector2 uvMin{ 0.0f, 0.0f }; //Atlas Texture의 좌상단 UV. 즉 source
    Core::Vector2 uvMax{ 0.0f, 0.0f }; //Atlas Texture의 우하단 UV

    uint16_t pageIndex{ 0 };
};

struct ShapedGlyph
{
    uint32_t glyphIndex;

    float advanceX;
    float offsetX;
    float offsetY;
};

class FontResource;
struct ShapedText
{
    FontResource* font{};
    uint32_t size{};

    std::vector<ShapedGlyph> glyphs;
};

struct GlyphUploadEntry // 하나의 글자를 아틀라스로 전송하기 위한 데이터 묶음
{
    uint16_t pageIndex;
    std::vector<uint8_t> pixelData; // CPU 비트맵 임시 버퍼
    uint32_t width{ 0 };
    uint32_t height{ 0 };
    uint32_t packX{ 0 };
    uint32_t packY{ 0 };
};