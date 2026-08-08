#include "pch.h"
#include "FontAtlasBucket.h"
#include "Resource/Brush/BrushResource.h"

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
}

void FontAtlasBucket::CreatePage()
{
    auto page = std::make_unique<AtlasPage>();

    page->Initialize(
        m_device,
        m_factory,
        m_atlasTextureSize,
        GetAtlasPageFormat());

    m_pages.push_back(std::move(page));
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

std::shared_ptr<IBrushResource> FontAtlasBucket::GetBrush(uint16_t pageIndex) const
{
    Assert(pageIndex < m_pages.size());
    return m_pages[pageIndex]->GetBrushResource();
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