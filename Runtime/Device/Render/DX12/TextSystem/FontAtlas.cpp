#include "pch.h"
#include "FontAtlas.h"
#include "Core/Utils/StringUtils.h"
#include "Resource/Font/FontResource.h"

namespace cm = Core::Math;

void FontAtlas::UpdateAtlasIfNeeded(std::shared_ptr<IFontResource> fontResource, std::string_view text)
{
    if (text.empty()) return;
    auto font = static_cast<FontResource*>(fontResource.get());

    size_t offset = 0;
    while (offset < text.size())
    {
        char32_t codepoint = Core::UTF8ToUTF32Char(text, offset);
        if (codepoint == U'\0')
            continue;

        if (m_glyphCache.Contains(font, codepoint))
            continue;

        FT_Face face = font->GetFace();
        if (FT_Load_Char(face, codepoint, FT_LOAD_RENDER))
        {
            Assert(false); // FreeType에서 Glyph 로드 실패
            continue;
        }

        FT_GlyphSlot slot = face->glyph;
        uint32_t width = slot->bitmap.width;
        uint32_t height = slot->bitmap.rows;

        auto [packX, packY] = m_packer.AllocateRect(width + 2, height + 2);

        GlyphInfo glyphInfo;
        glyphInfo.width = static_cast<float>(width);
        glyphInfo.height = static_cast<float>(height);
        glyphInfo.bearingX = static_cast<float>(slot->bitmap_left);
        glyphInfo.bearingY = static_cast<float>(slot->bitmap_top);
        glyphInfo.advanceX = static_cast<float>(slot->advance.x >> 6); // 26.6 고정소수점 변환

        // 아틀라스 텍스처 해상도 기준의 0.0 ~ 1.0 UV 좌표 계산
        float atlasW = static_cast<float>(m_atlasTextureSize.width);
        float atlasH = static_cast<float>(m_atlasTextureSize.height);

        glyphInfo.uvMin = cm::Vector2(static_cast<float>(packX) / atlasW, static_cast<float>(packY) / atlasH);
        glyphInfo.uvMax = cm::Vector2(static_cast<float>(packX + width) / atlasW, static_cast<float>(packY + height) / atlasH);

        m_glyphCache.Insert(font, codepoint, glyphInfo);
    }
}