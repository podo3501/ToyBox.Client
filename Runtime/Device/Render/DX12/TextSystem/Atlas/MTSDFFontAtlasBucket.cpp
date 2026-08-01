#include "pch.h"
#include "MTSDFFontAtlasBucket.h"
#include "Resource/Font/FontResource.h"
#include "Glyph/MTSDFGlyphGenerator.h"
#include "AtlasHelper.h"

MTSDFFontAtlasBucket::~MTSDFFontAtlasBucket() = default;
MTSDFFontAtlasBucket::MTSDFFontAtlasBucket(
    Device& device,
    DescriptorFactory& factory,
    FontBucketID bucketID) :
    m_device{ device },
    m_factory{ factory },
    m_bucketID{ bucketID }
{
}

void MTSDFFontAtlasBucket::Initialize(const Size& atlasTextureSize)
{
    Assert(atlasTextureSize.width > 0 && atlasTextureSize.height > 0);
    m_atlasTextureSize = atlasTextureSize;
}

void MTSDFFontAtlasBucket::CreatePage()
{
    auto page = std::make_unique<AtlasPage>();

    page->Initialize(
        m_device,
        m_factory,
        m_atlasTextureSize,
        GetAtlasPageDesc());

    m_pages.push_back(std::move(page));
}

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

        if (m_glyphCache.Contains(
            font,
            glyphIndex,
            shapedText.size))
        {
            continue;
        }

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

        GlyphUploadEntry upload;
        if (CreateUploadEntry(
            std::move(mtsdf.pixels),
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

const GlyphInfo* MTSDFFontAtlasBucket::FindGlyph(
    FontResource* font,
    uint32_t glyphIndex,
    uint32_t size) const
{
    return m_glyphCache.Get(
        font,
        glyphIndex,
        size);
}

std::shared_ptr<IMaterialResource> MTSDFFontAtlasBucket::GetMaterial(uint16_t pageIndex) const
{
    Assert(pageIndex < m_pages.size());
    return m_pages[pageIndex]->GetMaterialResource();
}

const Resource& MTSDFFontAtlasBucket::GetAtlasResource(uint16_t pageIndex) const
{
    Assert(pageIndex < m_pages.size());
    return m_pages[pageIndex]->GetAtlasResource();
}

uint16_t MTSDFFontAtlasBucket::CurrentPageIndex() const
{
    Assert(!m_pages.empty());
    return static_cast<uint16_t>(m_pages.size() - 1);
}