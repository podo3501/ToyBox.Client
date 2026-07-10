#include "pch.h"
#include "GlyphCache.h"

bool GlyphCache::Contains(FontResource* fontRes, char32_t codepoint, uint32_t size) const
{
    CacheKey key{ fontRes, codepoint, size };
    return m_cache.find(key) != m_cache.end();
}

void GlyphCache::Insert(FontResource* fontRes, char32_t codepoint, uint32_t size, const GlyphInfo& info)
{
    CacheKey key{ fontRes, codepoint, size };
    m_cache[key] = info;
}

const GlyphInfo* GlyphCache::Get(FontResource* fontRes, char32_t codepoint, uint32_t size) const
{
    CacheKey key{ fontRes, codepoint, size };
    auto it = m_cache.find(key);
    if (it != m_cache.end())
        return &it->second;

    return nullptr;
}

void GlyphCache::Clear()
{
    m_cache.clear();
}