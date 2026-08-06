#include "pch.h"
#include "GlyphRegistry.h"
#include "../TextTypes.h"

void GlyphRegistry::Register(RGResourceID resID, std::weak_ptr<GlyphInfo> info)
{
    m_pending[resID] = std::move(info);
}

void GlyphRegistry::MarkReady(RGResourceID resID)
{
    auto it = m_pending.find(resID);
    if (it == m_pending.end())
        return;

    if (auto info = it->second.lock()) // 캐시가 이미 Clear()됐다면 여기서 자연스럽게 nullptr
        info->isReady = true;

    m_pending.erase(it); // 처리 끝났으니 정리
}