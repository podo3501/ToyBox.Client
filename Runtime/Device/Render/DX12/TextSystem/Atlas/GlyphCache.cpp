#include "pch.h"
#include "GlyphCache.h"

void GlyphCache::Insert(FontResource* fontRes, uint32_t glyphIndex, uint32_t size, const GlyphInfo& info)
{
    CacheKey key{ fontRes, glyphIndex, size };
    m_cache[key] = info;
}

const GlyphInfo* GlyphCache::Get(FontResource* fontRes, uint32_t glyphIndex, uint32_t size) const
{
    CacheKey key{ fontRes, glyphIndex, size };
    auto it = m_cache.find(key);
    if (it != m_cache.end())
        return &it->second;

    return nullptr;
}

void GlyphCache::Clear()
{
    m_cache.clear();
}