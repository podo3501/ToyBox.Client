#include "pch.h"
#include "GlyphCache.h"

bool GlyphCache::Contains(FontResource* fontRes, char32_t codepoint) const
{
    CacheKey key{ fontRes, codepoint };
    return m_cache.find(key) != m_cache.end();
}

void GlyphCache::Insert(FontResource* fontRes, char32_t codepoint, const GlyphInfo& info)
{
    CacheKey key{ fontRes, codepoint };
    m_cache[key] = info;
}

GlyphInfo GlyphCache::Get(FontResource* fontRes, char32_t codepoint) const
{
    CacheKey key{ fontRes, codepoint };
    auto it = m_cache.find(key);
    if (it != m_cache.end())
    {
        return it->second;
    }

    return GlyphInfo{};
}

void GlyphCache::Clear()
{
    m_cache.clear();
}