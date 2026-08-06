#include "pch.h"
#include "MTSDFFontAtlasBucket.h"
#include "Resource/Font/FontResource.h"
#include "Glyph/MTSDFGlyphGenerator.h"
#include "AtlasHelper.h"

AtlasPageDesc MTSDFFontAtlasBucket::GetAtlasPageDesc() const
{
    return {
        GlyphPixelFormat::RGBA8,
        RegistryShader::UI
    };
}

void MTSDFFontAtlasBucket::EnsureGlyphs(
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

        MTSDFGlyph mtsdf = GenerateMTSDFGlyph(
            font,
            glyphIndex,
            shapedText.size);
        auto& pixels = mtsdf.pixels;
        auto& metrics = mtsdf.metrics;

        if (metrics.width == 0.f || metrics.height == 0.f)
        {
            m_glyphCache.Insert(
                font,
                glyphIndex,
                shapedText.size,
                CreateEmptyGlyphInfo(mtsdf));

            continue;
        }

        if (m_pages.empty()) //페이지가 클래스 생성때는 없다.
        {
            CreatePage();
            outUploadsPerPage.resize(m_pages.size());
        }

        Size packSize{
            pixels.width + Padding * 2,
            pixels.height + Padding * 2 };

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

        uint32_t bakeSize = GetMTSDFResolution(m_bucketID); // 이 버킷이 실제로 굽는 해상도
        float scale = static_cast<float>(shapedText.size) / static_cast<float>(bakeSize);

        auto glyphInfo =
            CreateGlyphInfo(
                mtsdf,
                m_bucketID,
                pageIndex,
                *packPos,
                Padding,
                m_atlasTextureSize,
                scale);

        std::shared_ptr<GlyphInfo> cachedInfo = m_glyphCache.Insert(
            font, glyphIndex, shapedText.size, glyphInfo);

        GlyphUploadEntry upload;
        if (CreateUploadEntry(
            std::move(mtsdf.pixels), 
            *packPos, 
            Padding, 
            upload))
        {
            upload.readyTarget = cachedInfo; // weak_ptr 대입 (shared_ptr -> weak_ptr 암시적 변환)
            outUploadsPerPage[pageIndex].push_back(std::move(upload));
        }
        else
            cachedInfo->isReady = true;
    }
}
