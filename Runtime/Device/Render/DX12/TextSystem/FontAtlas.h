#pragma once
#include "GameClient/Service/Render/Resource/IFontResource.h"
#include "GlyphCache.h"
#include "AtlasPacker.h"

class FontAtlas
{
public:
    void UpdateAtlasIfNeeded(std::shared_ptr<IFontResource> fontRes, std::string_view text);

private:
    Size m_atlasTextureSize{ 1024, 1024 }; //?!? 일단은 이렇게 하고 나중에 config에 넣어야 한다.

    GlyphCache m_glyphCache;
    AtlasPacker m_packer{ m_atlasTextureSize };
};
