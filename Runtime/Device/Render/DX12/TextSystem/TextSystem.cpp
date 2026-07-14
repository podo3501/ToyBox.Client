#include "pch.h"
#include "TextSystem.h"
#include "TextHelpers.h"
#include "Core/Device.h"
#include "Core/Utils/StringUtils.h"
#include "Resource/Font/FontResource.h"
#include "Resource/Texture/TextureResource.h"
#include "Resource/Mesh/TransientMeshResource.h"
#include "Factory/DescriptorFactory.h"
#include "Helpers/TextureHelpers.h"
#include "GameClient/Asset/MeshAsset.h"
#include "Provider/Mesh/TransientMeshProvider.h"

struct PageMesh
{
    uint16_t pageIndex{ 0 };
    std::shared_ptr<TransientMeshResource> mesh;
};

TextSystem::~TextSystem() = default;
TextSystem::TextSystem(
    Device& device,
    DescriptorFactory& factory,
    TaskScheduler& taskScheduler, 
    ResourceFactory& resFactory,
    TransientMeshProvider& transientMeshProvider) :
    m_device{ device },
    m_factory{ factory },
    m_atlasBuilder{ taskScheduler, resFactory },
    m_transientMeshProvider{ transientMeshProvider }
{}

bool TextSystem::Initialize(const Size& atlasTexSize)
{
    m_atlasTextureSize = atlasTexSize;
    CreatePage();

    return true;
}

void TextSystem::CreatePage()
{
    auto page = std::make_unique<AtlasPage>();
    page->Initialize(m_device, m_factory, m_atlasTextureSize);
    m_pages.push_back(std::move(page));
}

std::vector<DrawUIItem> TextSystem::BuildDrawItems(std::span<const DrawTextItem> items)
{
    std::vector<DrawUIItem> result;
    if (items.empty())
        return result;

    auto shapedTexts = ShapeTexts(items);
    std::vector<std::vector<GlyphUploadEntry>> uploadsPerPage; // 이번 프레임에 새롭게 아틀라스에 추가할 글자들의 목록
    uploadsPerPage.resize(m_pages.size());

    for (const auto& shaped : shapedTexts)
        EnsureGlyphs(shaped, uploadsPerPage);

    for (uint16_t pageIndex = 0; pageIndex < uploadsPerPage.size(); ++pageIndex)
    {
        auto& uploads = uploadsPerPage[pageIndex];
        if (uploads.empty())
            continue;

        m_atlasBuilder.UploadGlyphsToAtlas(
            m_pages[pageIndex]->GetAtlasResource(),
            uploads);
    }

    auto pageMeshes = CreateTextMesh(items, shapedTexts);
    if (pageMeshes.empty())
        return result;

    for (auto& pageMesh : pageMeshes)
    {
        DrawUIItem item;
        item.mesh = pageMesh.mesh;
        item.material = m_pages[pageMesh.pageIndex]->GetMaterialResource();

        result.push_back(std::move(item));
    }

    return result;
}

std::vector<ShapedText> TextSystem::ShapeTexts(std::span<const DrawTextItem> items)
{
    std::vector<ShapedText> result;
    result.reserve(items.size());

    for (const auto& item : items)
    {
        auto font = static_cast<FontResource*>(item.fontRes.get());

        ShapedText shaped;
        shaped.font = font;
        shaped.size = item.fontSize;
        shaped.glyphs = font->Shape(item.codePoints, item.fontSize);

        result.push_back(std::move(shaped));
    }

    return result;
}

struct TextMeshBuilder
{
    std::vector<UIVertex> vertices;
    std::vector<uint32_t> indices;
    uint32_t vertexOffset = 0;
};

std::vector<PageMesh> TextSystem::CreateTextMesh(
    std::span<const DrawTextItem> items,
    std::span<const ShapedText> shapedTexts)
{
    size_t totalGlyphCount = 0;
    for (const auto& shapedText : shapedTexts)
        totalGlyphCount += shapedText.glyphs.size();

    if (totalGlyphCount == 0)
        return {};

    std::vector<TextMeshBuilder> builders(m_pages.size());

    for (auto& builder : builders)
    {
        builder.vertices.reserve(totalGlyphCount * 4 / builders.size());
        builder.indices.reserve(totalGlyphCount * 6 / builders.size());
    }

    for (size_t itemIndex = 0; itemIndex < items.size(); ++itemIndex)
    {
        const auto& item = items[itemIndex];
        const auto& shapedText = shapedTexts[itemIndex];
        auto font = shapedText.font;

        float cursorX = item.position.x;
        float baselineY = item.position.y;

        for (const auto& shapedGlyph : shapedText.glyphs)
        {
            const GlyphInfo* glyph = m_glyphCache.Get(font, shapedGlyph.glyphIndex, shapedText.size);
            if (!glyph)
                continue;

            if (glyph->width == 0.f || glyph->height == 0.f)
            {
                cursorX += shapedGlyph.advanceX;
                continue;
            }

            auto& builder = builders[glyph->pageIndex];
            float x = cursorX + glyph->bearingX + shapedGlyph.offsetX;
            float y = baselineY - glyph->bearingY - shapedGlyph.offsetY;

            AppendGlyphQuad(
                builder.vertices,
                builder.indices,
                builder.vertexOffset,
                *glyph,
                x,
                y,
                item.color);

            cursorX += shapedGlyph.advanceX;
        }
    }

    std::vector<PageMesh> result;
    result.reserve(builders.size());

    for (uint16_t pageIndex = 0; pageIndex < builders.size(); ++pageIndex)
    {
        auto& builder = builders[pageIndex];
        if (builder.vertices.empty())
            continue;

        auto mesh = m_transientMeshProvider.Create(builder.vertices, builder.indices);
        if (!mesh)
            continue;

        result.push_back({
            pageIndex,
            std::move(mesh)
            });
    }

    return result;
}

void TextSystem::EnsureGlyphs(
    const ShapedText& shapedText,
    std::vector<std::vector<GlyphUploadEntry>>& outUploads)
{
    constexpr uint32_t Padding = 1;
    auto font = shapedText.font;

    for (const auto& glyph : shapedText.glyphs)
    {
        uint32_t glyphIndex = glyph.glyphIndex;
        if (m_glyphCache.Contains(font, glyphIndex, shapedText.size))
            continue;

        FT_GlyphSlot slot = font->GetGlyphSlot(glyphIndex, shapedText.size);
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

        Size packSize{ width + Padding * 2, height + Padding * 2 };
        uint16_t pageIdx = CurrentPageIndex();
        auto packPos = m_pages[pageIdx]->AllocateRect(packSize);

        if (packPos.x < 0)
        {
            CreatePage();
            outUploads.resize(m_pages.size());

            pageIdx = CurrentPageIndex();
            packPos = m_pages[pageIdx]->AllocateRect(packSize);
        }
        auto& currentPage = m_pages[pageIdx];

        GlyphUploadEntry uploadEntry;
        uploadEntry.pageIndex = pageIdx;

        if (CreateUploadEntry(slot, packPos.x, packPos.y, Padding, uploadEntry))
            outUploads[pageIdx].push_back(std::move(uploadEntry));

        auto glyphInfo = CreateGlyphInfo(
            slot,
            packPos.x,
            packPos.y,
            Padding,
            m_atlasTextureSize,
            pageIdx);

        m_glyphCache.Insert(font, glyphIndex, shapedText.size, glyphInfo);
    }
}

uint16_t TextSystem::CurrentPageIndex() const
{
    return static_cast<uint16_t>(m_pages.size() - 1);
}