#pragma once
#include "Graph/RGTypes.h"

struct GlyphInfo;

class GlyphRegistry
{
public:
    void Register(RGResourceID resID, std::weak_ptr<GlyphInfo> info);
    void MarkReady(RGResourceID resID);

private:
    std::unordered_map<RGResourceID, std::weak_ptr<GlyphInfo>> m_pending;
};
