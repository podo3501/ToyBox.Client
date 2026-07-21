#pragma once
#include <cstdint>

using FontBucketID = uint32_t;
constexpr FontBucketID InvalidFontBucket = 0xffffffff;

namespace BitmapBuckets
{
    constexpr FontBucketID Small = 8;
    constexpr FontBucketID Medium = 14;
    constexpr FontBucketID Large = 20;
}

namespace SDFBuckets
{
    constexpr FontBucketID Small = 20;
    constexpr FontBucketID Medium = 30;
    constexpr FontBucketID Large = 40;
    constexpr FontBucketID Huge = 50;
}