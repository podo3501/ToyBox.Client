#pragma once
#include "TextTypes.h"
#include "Core/Utils/Hash.h"

class FontResource;

class GlyphCache
{
public:
    bool Contains(FontResource* fontRes, char32_t codepoint, uint32_t size) const;
    void Insert(FontResource* fontRes, char32_t codepoint, uint32_t size, const GlyphInfo& info);
    const GlyphInfo* Get(FontResource* fontRes, char32_t codepoint, uint32_t size) const;
    void Clear();

private:
    struct CacheKey
    {
        FontResource* fontRes{ nullptr }; // 검색 속도를 극대화하기 위해 폰트 포인터(Raw주소)와 코드포인트를 64비트 키 하나로 결합
        char32_t codepoint{};
        uint32_t size{ 0 };

        bool operator==(const CacheKey& other) const = default;
    };

    struct CacheKeyHash
    {
        std::size_t operator()(const CacheKey& key) const
        {
            return Core::HashOf(
                key.fontRes, 
                key.codepoint, 
                key.size);
        }
    };

    std::unordered_map<CacheKey, GlyphInfo, CacheKeyHash> m_cache;
};
