#pragma once
#include <cstdint>

using FontBucketID = uint32_t;
constexpr FontBucketID InvalidFontBucket = 0xffffffff;

namespace FontBuckets
{
    constexpr FontBucketID Small = 16;
    constexpr FontBucketID Medium = 24;
    constexpr FontBucketID Large = 32;
    constexpr FontBucketID Huge = 48;
}

inline FontBucketID GetFontBucketID(uint32_t size)
{
    if (size <= FontBuckets::Small) return FontBuckets::Small;
    if (size <= FontBuckets::Medium) return FontBuckets::Medium;
    if (size <= FontBuckets::Large) return FontBuckets::Large;
    
    return FontBuckets::Huge;
}