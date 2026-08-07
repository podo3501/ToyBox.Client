#pragma once
#include "../TextTypes.h"
#include "Core/Utils/Hash.h"

class FontResource;

class GlyphCache
{
public:
    void Insert(FontResource* fontRes, uint32_t glyphIndex, uint32_t size, const GlyphInfo& info);
    const GlyphInfo* Get(FontResource* fontRes, uint32_t glyphIndex, uint32_t size) const;
    void Clear();

private:
    struct CacheKey
    {
        FontResource* fontRes{ nullptr }; // 검색 속도를 극대화하기 위해 폰트 포인터(Raw주소)와 코드포인트를 64비트 키 하나로 결합
        uint32_t glyphIndex{};
        uint32_t size{ 0 };

        bool operator==(const CacheKey&) const = default;
    };

    struct CacheKeyHash
    {
        std::size_t operator()(const CacheKey& key) const
        {
            return Core::HashOf(
                key.fontRes, 
                key.glyphIndex, 
                key.size);
        }
    };

    std::unordered_map<CacheKey, GlyphInfo, CacheKeyHash> m_cache;
};
