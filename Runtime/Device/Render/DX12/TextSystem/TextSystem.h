#pragma once
#include "GameClient/Service/Render/Resource/IFontResource.h"
#include "FontAtlas.h"
#include "Core/RenderData.h"

class TextSystem
{
public:
    std::vector<DrawUIItem> DrawText(
        std::shared_ptr<IFontResource> fontRes,
        std::string_view text,
        const Vector2& pos);

private:
    FontAtlas m_fontAtlas;
};
