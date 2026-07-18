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

namespace FontSDF
{
    constexpr uint32_t Small = 32;
    constexpr uint32_t Medium = 48;
    constexpr uint32_t Large = 64;
    constexpr uint32_t Huge = 96;
}

inline uint32_t GetSDFResolution(FontBucketID bucket)
{
    switch (bucket)
    {
    case FontBuckets::Small:  return FontSDF::Small;
    case FontBuckets::Medium: return FontSDF::Medium;
    case FontBuckets::Large:  return FontSDF::Large;
    case FontBuckets::Huge:   return FontSDF::Huge;
    default:
        Assert(false);
        return FontSDF::Large;
    }
}

inline double GetSDFRange(FontBucketID bucket)
{
    switch (bucket)
    {
    case FontBuckets::Small: return 3.0;
    case FontBuckets::Medium: return 4.0;
    case FontBuckets::Large: return 6.0;
    case FontBuckets::Huge: return 8.0;
    }

    return 4.0;
}