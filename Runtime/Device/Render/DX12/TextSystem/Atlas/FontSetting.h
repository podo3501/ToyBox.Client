#pragma once
#include <cstdint>
#include "GameClient/Service/Render/Definition/Text/TextStyle.h"

//폰트 크기는 atlas 페이지를 나누는 기준이 된다.
//bitmap은 크기별로 다 찍고 구간에 한해서 한곳에 모아진다(9 -14 까지는 medium page에)
//MTSDF는 하나의 크기만 찍고 그걸 줄이거나 늘려서 사용한다. 그래서 페이지 절약이 된다.
//20폰트 이하는 MTSDF를 쓸 필요가 없다면(아웃라인이나 줌아웃 같은게 필요 없다면) bitmap이 실용적이다.
//실제로는 bitmap과 SDF도 작은 폰트에도 그렇게 차이가 없기 때문에 일괄적으로 MTSDF로 해도 되긴 한다. 테스트 해 보고 쓰는 사람이 결정하도록 한다.

using FontBucketID = uint32_t;
constexpr FontBucketID InvalidFontBucket = 0xffffffff;

namespace BitmapBuckets
{
    constexpr FontBucketID Small = 8;
    constexpr FontBucketID Medium = 14;
    constexpr FontBucketID Large = 20;
}

namespace MTSDFBuckets
{
    constexpr FontBucketID Small = 24;
    constexpr FontBucketID Medium = 60;
}

inline FontBucketID GetFontBucketID(TextRenderMode mode, uint32_t size)
{
    switch (mode)
    {
    case TextRenderMode::Bitmap: 
        if (size <= BitmapBuckets::Small) return BitmapBuckets::Small;
        if (size <= BitmapBuckets::Medium) return BitmapBuckets::Medium;
        if (size <= BitmapBuckets::Large) return BitmapBuckets::Large;
        break;
    case TextRenderMode::MTSDF:
        if (size <= MTSDFBuckets::Small) return MTSDFBuckets::Small;
        return MTSDFBuckets::Medium;
        break;
    }
    Assert(false);

    return InvalidFontBucket;
}

inline uint32_t GetMTSDFResolution(uint32_t size)
{
    return static_cast<uint32_t>(GetFontBucketID(TextRenderMode::MTSDF, size));
}