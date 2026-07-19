#include "pch.h"
#include "FontAtlasBucket.h"
#include "Glyph/GlyphGenerator.h"
#include "Resource/Font/FontResource.h"
#include "../TextHelpers.h"

FontAtlasBucket::~FontAtlasBucket() = default;
FontAtlasBucket::FontAtlasBucket(
    Device& device,
    DescriptorFactory& factory,
    std::unique_ptr<GlyphGenerator> glyphGenerator,
    FontBucketID bucketID) :
    m_device{ device },
    m_factory{ factory },
    m_glyphGenerator{ std::move(glyphGenerator) },
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

        GlyphBitmap bitmap = m_glyphGenerator->Generate(
            font,
            glyphIndex,
            shapedText.size);

        Core::Logger::Info(
            "Glyph {} image={}x{} glyph={}x{} channels={}",
            glyphIndex,
            bitmap.width,
            bitmap.height,
            bitmap.glyphWidth,
            bitmap.glyphHeight,
            bitmap.channels);

        if (bitmap.width == 0 || bitmap.height == 0)
        {
            m_glyphCache.Insert(
                font,
                glyphIndex,
                shapedText.size,
                CreateEmptyGlyphInfo(bitmap));

            continue;
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

        auto glyphInfo =
            CreateGlyphInfo(
                bitmap,
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

std::shared_ptr<IMaterialResource> FontAtlasBucket::GetMaterial(uint16_t pageIndex) const
{
    Assert(pageIndex < m_pages.size());
    return m_pages[pageIndex]->GetMaterialResource();
}

const Resource& FontAtlasBucket::GetAtlasResource(uint16_t pageIndex) const
{
    Assert(pageIndex < m_pages.size());
    return m_pages[pageIndex]->GetAtlasResource();
}

uint16_t FontAtlasBucket::CurrentPageIndex() const
{
    Assert(!m_pages.empty());
    return static_cast<uint16_t>(m_pages.size() - 1);
}