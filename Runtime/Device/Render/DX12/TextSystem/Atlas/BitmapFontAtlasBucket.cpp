#include "pch.h"
#include "BitmapFontAtlasBucket.h"
#include "Resource/Font/FontResource.h"
#include "Glyph/BitmapGlyphGenerator.h"
#include "AtlasHelper.h"

AtlasPageDesc BitmapFontAtlasBucket::GetAtlasPageDesc() const
{
    return {
        GlyphPixelFormat::R8,
        RegistryShader::UI
    };
}

void BitmapFontAtlasBucket::EnsureGlyphs(
    const ShapedText& shapedText,
    std::vector<std::vector<GlyphUploadEntry>>& outUploadsPerPage)
{
    constexpr uint32_t Padding = 1;

    outUploadsPerPage.resize(m_pages.size());
    auto font = shapedText.font;
    for (const auto& shapedGlyph : shapedText.glyphs)
    {
        uint32_t glyphIndex = shapedGlyph.glyphIndex;
        if (m_glyphCache.Get(font, glyphIndex, shapedText.size))
            continue;

        BitmapGlyph bitmap = GenerateBitmapGlyph(
            font,
            glyphIndex,
            shapedText.size);

        if (bitmap.metrics.width == 0 || bitmap.metrics.height == 0)
        {
            m_glyphCache.Insert(
                font,
                glyphIndex,
                shapedText.size,
                CreateEmptyGlyphInfo(bitmap));

            continue;
        }

        if (m_pages.empty()) //페이지가 클래스 생성때는 없다.
        {
            CreatePage();
            outUploadsPerPage.resize(m_pages.size());
        }

        Size packSize{ 
            bitmap.pixels.width + Padding * 2,
            bitmap.pixels.height + Padding * 2 };

        uint16_t pageIndex = CurrentPageIndex();
        auto packPos = m_pages[pageIndex]->AllocateRect(packSize);

        if (!packPos)
        {
            CreatePage();
            outUploadsPerPage.resize(m_pages.size());

            pageIndex = CurrentPageIndex();
            packPos = m_pages[pageIndex]->AllocateRect(packSize);
            Assert(packPos);
        }

        auto glyphInfo =
            CreateGlyphInfo(
                bitmap,
                m_bucketID,
                pageIndex,
                *packPos,
                Padding,
                m_atlasTextureSize);

        GlyphUploadEntry upload;
        if (CreateUploadEntry(
            std::move(bitmap.pixels),
            *packPos,
            Padding,
            upload))
        {
            outUploadsPerPage[pageIndex].push_back(
                std::move(upload));
        }

        m_glyphCache.Insert(
            font,
            glyphIndex,
            shapedText.size,
            glyphInfo);
    }
}