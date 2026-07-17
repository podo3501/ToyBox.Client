#include "pch.h"
#include "FontAtlasBucket.h"
#include "Resource/Font/FontResource.h"
#include "../TextHelpers.h"

FontAtlasBucket::~FontAtlasBucket() = default;
FontAtlasBucket::FontAtlasBucket(
    Device& device,
    DescriptorFactory& factory,
    FontBucketID bucketID) :
    m_device{ device },
    m_factory{ factory },
    m_bucketID{ bucketID }
{}

void FontAtlasBucket::Initialize(const Size& atlasTextureSize)
{
    Assert(atlasTextureSize.width > 0 && atlasTextureSize.height > 0);

    m_atlasTextureSize = atlasTextureSize;
    CreatePage();
}

void FontAtlasBucket::CreatePage()
{
    auto page = std::make_unique<AtlasPage>();

    page->Initialize(
        m_device,
        m_factory,
        m_atlasTextureSize);

    m_pages.push_back(std::move(page));
}

void FontAtlasBucket::EnsureGlyphs(
    const ShapedText& shapedText,
    std::vector<std::vector<GlyphUploadEntry>>& outUploadsPerPage)
{
    constexpr uint32_t Padding = 1;

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

        FT_GlyphSlot slot =
            font->GetGlyphSlot(
                glyphIndex,
                shapedText.size);

        Assert(slot);

        uint32_t width = slot->bitmap.width;
        uint32_t height = slot->bitmap.rows;

        if (width == 0 || height == 0)
        {
            m_glyphCache.Insert(
                font,
                glyphIndex,
                shapedText.size,
                CreateEmptyGlyphInfo(slot));

            continue;
        }

        Size packSize{
            width + Padding * 2,
            height + Padding * 2
        };

        uint16_t pageIndex = CurrentPageIndex();

        auto packPos =
            m_pages[pageIndex]->AllocateRect(packSize);

        if (!packPos)
        {
            CreatePage();

            outUploadsPerPage.resize(m_pages.size());

            pageIndex = CurrentPageIndex();

            packPos =
                m_pages[pageIndex]->AllocateRect(packSize);

            Assert(packPos);
        }

        GlyphUploadEntry uploadEntry;
        if (CreateUploadEntry(
            slot,
            m_bucketID,
            pageIndex,
            packPos->x,
            packPos->y,
            Padding,
            uploadEntry))
        {
            outUploadsPerPage[pageIndex].push_back(
                std::move(uploadEntry));
        }

        auto glyphInfo = CreateGlyphInfo(
            slot,
            m_bucketID,
            pageIndex,
            packPos->x,
            packPos->y,
            Padding,
            m_atlasTextureSize);

        m_glyphCache.Insert(
            font,
            glyphIndex,
            shapedText.size,
            glyphInfo);
    }
}

const GlyphInfo* FontAtlasBucket::FindGlyph(
    FontResource* font,
    uint32_t glyphIndex,
    uint32_t size) const
{
    return m_glyphCache.Get(
        font,
        glyphIndex,
        size);
}

std::shared_ptr<IMaterialResource>
FontAtlasBucket::GetMaterial(
    uint16_t pageIndex) const
{
    Assert(pageIndex < m_pages.size());

    return m_pages[pageIndex]->GetMaterialResource();
}

const Resource&
FontAtlasBucket::GetAtlasResource(
    uint16_t pageIndex) const
{
    Assert(pageIndex < m_pages.size());

    return m_pages[pageIndex]->GetAtlasResource();
}

uint16_t FontAtlasBucket::PageCount() const
{
    return static_cast<uint16_t>(m_pages.size());
}

uint16_t FontAtlasBucket::CurrentPageIndex() const
{
    Assert(!m_pages.empty());

    return static_cast<uint16_t>(m_pages.size() - 1);
}