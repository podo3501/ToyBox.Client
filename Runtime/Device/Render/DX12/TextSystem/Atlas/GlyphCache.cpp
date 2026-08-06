#include "pch.h"
#include "GlyphCache.h"

std::shared_ptr<GlyphInfo> GlyphCache::Insert(
    FontResource* fontRes, 
    uint32_t glyphIndex, 
    uint32_t size, 
    const GlyphInfo& info)
{
    CacheKey key{ fontRes, glyphIndex, size };
    auto sp = std::make_shared<GlyphInfo>(info);
    m_cache[key] = sp;
    return sp;
}

const GlyphInfo* GlyphCache::Get(FontResource* fontRes, uint32_t glyphIndex, uint32_t size) const
{
    CacheKey key{ fontRes, glyphIndex, size };
    auto it = m_cache.find(key);
    if (it != m_cache.end())
        return it->second.get();

    return nullptr;
}

void GlyphCache::Clear()
{
    m_cache.clear(); // shared_ptr들이 참조 카운트만 줄어듦. 아직 살아있는 weak_ptr은 자동으로 expired 처리됨
}