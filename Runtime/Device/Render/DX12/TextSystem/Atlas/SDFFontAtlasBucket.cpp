#include "pch.h"
#include "SDFFontAtlasBucket.h"
#include "Glyph/GlyphGenerator.h"
#include "Resource/Font/FontResource.h"
#include "../TextHelpers.h"

SDFFontAtlasBucket::~SDFFontAtlasBucket() = default;
SDFFontAtlasBucket::SDFFontAtlasBucket(
    Device& device,
    DescriptorFactory& factory,
    FontBucketID bucketID) :
    m_device{ device },
    m_factory{ factory },
    m_bucketID{ bucketID }
{}

void SDFFontAtlasBucket::Initialize(const Size& atlasTextureSize)
{
    Assert(atlasTextureSize.width > 0 && atlasTextureSize.height > 0);
    m_atlasTextureSize = atlasTextureSize;
}

void SDFFontAtlasBucket::CreatePage()
{
    auto page = std::make_unique<AtlasPage>();

    page->Initialize(
        m_device,
        m_factory,
        m_atlasTextureSize);

    m_pages.push_back(std::move(page));
}

void SDFFontAtlasBucket::EnsureGlyphs(
    const ShapedText& shapedText,
    std::vector<std::vector<GlyphUploadEntry>>& outUploadsPerPage)
{
    constexpr uint32_t Padding = 1;

    outUploadsPerPage.resize(m_pages.size());
    auto font = shapedText.font;
    for (const auto& shapedGlyph : shapedText.glyphs)
    {
        uint32_t glyphIndex = shapedGlyph.glyphIndex;

        if (m_glyphCache.Contains(
            font,
            glyphIndex,
            shapedText.size))
        {
            continue;
        }

        SDFGlyphBitmap sdfBitmap = m_glyphGenerator.Generate(
            font,
            glyphIndex,
            shapedText.size);
        auto& bitmap = sdfBitmap.bitmap;

        if (bitmap.width == 0 || bitmap.height == 0)
        {
            m_glyphCache.Insert(
                font,
                glyphIndex,
                shapedText.size,
                CreateEmptyGlyphInfo(sdfBitmap));

            continue;
        }

        if (m_pages.empty()) //페이지가 클래스 생성때는 없다.
        {
            CreatePage();
            outUploadsPerPage.resize(m_pages.size());
        }

        Size packSize{ 
            bitmap.width + Padding * 2, 
            bitmap.height + Padding * 2 };

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
                sdfBitmap,
                m_bucketID,
                pageIndex,
                packPos->x,
                packPos->y,
                Padding,
                m_atlasTextureSize);

        GlyphUploadEntry upload;
        if (CreateUploadEntry(
            std::move(bitmap),
            m_bucketID,
            pageIndex,
            packPos->x,
            packPos->y,
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

const GlyphInfo* SDFFontAtlasBucket::FindGlyph(
    FontResource* font,
    uint32_t glyphIndex,
    uint32_t size) const
{
    return m_glyphCache.Get(
        font,
        glyphIndex,
        size);
}

std::shared_ptr<IMaterialResource> SDFFontAtlasBucket::GetMaterial(uint16_t pageIndex) const
{
    Assert(pageIndex < m_pages.size());
    return m_pages[pageIndex]->GetMaterialResource();
}

const Resource& SDFFontAtlasBucket::GetAtlasResource(uint16_t pageIndex) const
{
    Assert(pageIndex < m_pages.size());
    return m_pages[pageIndex]->GetAtlasResource();
}

uint16_t SDFFontAtlasBucket::CurrentPageIndex() const
{
    Assert(!m_pages.empty());
    return static_cast<uint16_t>(m_pages.size() - 1);
}