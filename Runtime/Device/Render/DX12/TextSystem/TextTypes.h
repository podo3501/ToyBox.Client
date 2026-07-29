#pragma once
#include "Core/Math/Vector2.h"
#include "Atlas/FontSetting.h"
#include "Atlas/Glyph/GlyphTypes.h"
#include "GameClient/Service/Render/Definition/Text/TextStyle.h"

struct GlyphInfo
{
    TextRenderMode mode{ TextRenderMode::MTSDF };
    FontBucketID bucketID{ InvalidFontBucket };
    uint16_t pageIndex{ 0 };

    float width{ 0.0f }; // 그려질 글자의 크기. bitmap은 보여지는 것과 동일하지만, sdf는 거리장을 더한 만큼 크게 그린다. 즉 글자 사각형이 겹쳐지게 그려진다.
    float height{ 0.0f };
    float bearingX{ 0.0f }; //pen 위치에서 비트맵 왼쪽까지의 거리. pen은 현재 쓰여질 위치.
    float bearingY{ 0.0f }; //pen 위치에서 위쪽까지의 거리
    float pxRange{ 0.0f }; //range 값.
    
    Core::Vector2 uvMin{ 0.0f, 0.0f }; //Atlas Texture의 좌상단 UV. 즉 source
    Core::Vector2 uvMax{ 0.0f, 0.0f }; //Atlas Texture의 우하단 UV
    //?!? advanceX는 어디?  이 값이 있어야 하는지 어떤지 나중에 판단 해 보자.
};

struct ShapedGlyph
{
    uint32_t glyphIndex;
    uint32_t sourceIndex;
    uint32_t runIndex;
    uint32_t lineIndex;

    char32_t codepoint{ 0 };
    float advanceX;
    float offsetX;
    float offsetY;
};

class FontResource;
struct ShapedText
{
    FontResource* font{};
    TextRenderMode mode{ TextRenderMode::MTSDF };
    uint32_t size{};
    size_t index{ 0 }; //원본 텍스트 위치(여러 mode가 같이 그려지기 때문에)

    std::vector<ShapedGlyph> glyphs;
};

struct GlyphUploadEntry // 하나의 글자를 아틀라스로 전송하기 위한 데이터 묶음
{
    //atlas 위치
    uint32_t x{ 0 };
    uint32_t y{ 0 };

    GlyphPixels pixels; //업로드할 이미지 + glyph matric
};