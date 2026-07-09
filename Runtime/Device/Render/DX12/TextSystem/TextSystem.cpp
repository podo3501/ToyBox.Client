#include "pch.h"
#include "TextSystem.h"

std::vector<DrawUIItem> TextSystem::DrawText(
    std::shared_ptr<IFontResource> fontRes,
    std::string_view text,
    const Vector2& pos)
{
    std::vector<DrawUIItem> result;
    if (text.empty()) return result;

    m_fontAtlas.UpdateAtlasIfNeeded(fontRes, text);
    
    return result;
}