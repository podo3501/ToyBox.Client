#pragma once
#include "TextTypes.h"

class FontResource;

class GlyphCache
{
public:
    bool Contains(FontResource* fontRes, char32_t codepoint) const;
    void Insert(FontResource* fontRes, char32_t codepoint, const GlyphInfo& info);
    GlyphInfo Get(FontResource* fontRes, char32_t codepoint) const;
    void Clear();

private:
    struct CacheKey
    {
        FontResource* fontRes{ nullptr }; // 검색 속도를 극대화하기 위해 폰트 포인터(Raw주소)와 코드포인트를 64비트 키 하나로 결합
        char32_t codepoint{};

        bool operator==(const CacheKey& other) const
        {
            return fontRes == other.fontRes && codepoint == other.codepoint;
        }
    };

    struct CacheKeyHash
    {
        std::size_t operator()(const CacheKey& key) const
        {
            std::size_t h1 = std::hash<FontResource*>{}(key.fontRes);
            std::size_t h2 = std::hash<uint32_t>{}(static_cast<uint32_t>(key.codepoint));
            return h1 ^ (h2 << 1);
        }
    };

    std::unordered_map<CacheKey, GlyphInfo, CacheKeyHash> m_cache;
};
