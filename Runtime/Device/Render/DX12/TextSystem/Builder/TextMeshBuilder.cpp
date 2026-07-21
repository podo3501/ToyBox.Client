#include "pch.h"
#include "TextMeshBuilder.h"
#include "../Atlas/FontAtlas.h"
#include "../TextHelpers.h"
#include "Provider/Mesh/TransientMeshProvider.h"
#include "Core/Utils/Hash.h"

struct PageMeshBuffer
{
    std::vector<UIVertex> vertices;
    std::vector<uint32_t> indices;
    uint32_t vertexOffset = 0;

    std::shared_ptr<IMaterialResource> material{ nullptr };
};

struct TextBatchKey
{
    FontBucketID bucket{ InvalidFontBucket };
    uint16_t pageIndex{ 0 };

    bool operator==(const TextBatchKey&) const = default;
};

struct TextBatchKeyHash
{
    size_t operator()(const TextBatchKey& key) const
    {
        return Core::HashOf(
            key.bucket,
            key.pageIndex);
    }
};

TextMeshBuilder::TextMeshBuilder(TransientMeshProvider& meshProvider) :
    m_meshProvider{ meshProvider }
{}

std::vector<PageMesh> TextMeshBuilder::Build(
    const FontAtlas& atlas,
    std::span<const DrawTextItem> items,
    std::span<const ShapedText> shapedTexts)
{
    size_t totalGlyphCount = 0;

    for (const auto& shaped : shapedTexts)
        totalGlyphCount += shaped.glyphs.size();
    if (totalGlyphCount == 0)
        return {};

    std::unordered_map<TextBatchKey, PageMeshBuffer, TextBatchKeyHash> buffers;
    for (size_t itemIndex = 0; itemIndex < items.size(); ++itemIndex)
    {
        const auto& shaped = shapedTexts[itemIndex];
        const auto& item = items[shaped.index];

        float cursorX = item.position.x;
        float baselineY = item.position.y;

        for (const auto& shapedGlyph : shaped.glyphs)
        {
            const GlyphInfo* glyph = atlas.FindGlyph(
                shaped.font,
                shaped.mode,
                shapedGlyph.glyphIndex,
                shaped.size);
            if (!glyph)
            {
                cursorX += shapedGlyph.advanceX;
                continue;
            }

            if (glyph->width == 0 || glyph->height == 0)
            {
                cursorX += shapedGlyph.advanceX;
                continue;
            }

            TextBatchKey key
            {
                glyph->bucketID,
                glyph->pageIndex
            };

            auto& buffer = buffers[key];
            if (!buffer.material)
                buffer.material = atlas.GetMaterial(glyph);

            float x = cursorX + glyph->bearingX + shapedGlyph.offsetX;
            float y = baselineY - glyph->bearingY - shapedGlyph.offsetY;

            AppendGlyphQuad(
                buffer.vertices,
                buffer.indices,
                buffer.vertexOffset,
                *glyph,
                x,
                y,
                item.style.color);

            cursorX += shapedGlyph.advanceX;
        }
    }

    std::vector<PageMesh> result;
    result.reserve(buffers.size());

    for (auto& [key, buffer] : buffers)
    {
        if (buffer.vertices.empty())
            continue;

        auto mesh = m_meshProvider.Create(buffer.vertices, buffer.indices);
        if (!mesh)
            continue;

        result.push_back({
            std::move(mesh),
            std::move(buffer.material)
            });
    }

    return result;
}