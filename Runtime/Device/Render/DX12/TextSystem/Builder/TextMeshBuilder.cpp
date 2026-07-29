#include "pch.h"
#include "TextMeshBuilder.h"
#include "../Atlas/FontAtlas.h"
#include "../TextHelpers.h"
#include "Provider/Mesh/TransientMeshProvider.h"
#include "Core/Utils/Hash.h"
#include "Resource/Material/UIMaterialResource.h"
#include "Resource/Font/FontResource.h"

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

using TextBatchBufferMap = std::unordered_map<TextBatchKey, PageMeshBuffer, TextBatchKeyHash>;

static void ProcessShapedText(
    const FontAtlas& atlas,
    const ShapedText& shaped,
    const DrawTextItem& item,
    TextBatchBufferMap& buffers)
{
    float lineHeight = shaped.font->GetLineHeight(shaped.size);
    float ascent = shaped.font->GetAscent(shaped.size);

    uint32_t maxLines = (item.size.y > 0.f)
        ? static_cast<uint32_t>(item.size.y / lineHeight)
        : UINT32_MAX;

    float cursorX = item.position.x;
    float baselineY = item.position.y + ascent;
    uint32_t currentLine = 0;

    for (const auto& shapedGlyph : shaped.glyphs)
    {
        if (shapedGlyph.lineIndex != currentLine)
        {
            currentLine = shapedGlyph.lineIndex;
            cursorX = item.position.x;
            baselineY = item.position.y + ascent + currentLine * lineHeight;
        }

        if (currentLine >= maxLines)
            continue; // 세로 영역을 넘어간 줄은 그리지 않음

        const GlyphInfo* glyph = atlas.FindGlyph(
            shaped.font,
            shaped.mode,
            shapedGlyph.glyphIndex,
            shaped.size);
        if (!glyph || glyph->width == 0 || glyph->height == 0)
        {
            cursorX += shapedGlyph.advanceX;
            continue;
        }

        // width 클리핑: 박스 오른쪽을 넘어가면 스킵 (wrap이 켜져 있으면 이론상 안 넘어야 하나, 안전망으로)
        if (item.size.x > 0.f && (cursorX - item.position.x) > item.size.x)
        {
            cursorX += shapedGlyph.advanceX;
            continue;
        }

        TextBatchKey key
        {
            glyph->bucketID,
            glyph->pageIndex
        };

        auto material = atlas.GetMaterial(glyph);
        auto& buffer = buffers[key];
        if (!buffer.material)
            buffer.material = material;

        float x = cursorX + glyph->bearingX + shapedGlyph.offsetX;
        float y = baselineY - glyph->bearingY - shapedGlyph.offsetY;

        if (shaped.mode == TextRenderMode::Bitmap) //비트맵일때 반올림을 해서(정수로 픽셀을 맞춘다) 글자가 또렷하게 나오게 한다. MTSDF는 관계 없음.
        {
            x = std::floor(x + 0.5f);
            y = std::floor(y + 0.5f);
        }

        UIMaterialResource* uiMat = static_cast<UIMaterialResource*>(material.get());
        auto texIndices = uiMat->GetTextureIndices();

        const TextStyle& style = item.runs[shapedGlyph.runIndex].style;
        AppendGlyphQuad(
            buffer.vertices,
            buffer.indices,
            buffer.vertexOffset,
            *glyph,
            x,
            y,
            texIndices[0],
            style);

        cursorX += shapedGlyph.advanceX;
    }
}

static std::vector<PageMesh> CreatePageMeshes(
    TransientMeshProvider& meshProvider,
    TextBatchBufferMap& buffers)
{
    std::vector<PageMesh> result;
    result.reserve(buffers.size());

    for (auto& [key, buffer] : buffers)
    {
        if (buffer.vertices.empty())
            continue;

        auto mesh = meshProvider.Create(buffer.vertices, buffer.indices);
        if (!mesh)
            continue;

        result.push_back({
            std::move(mesh),
            std::move(buffer.material)
            });
    }

    return result;
}

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

    TextBatchBufferMap buffers;
    for(const auto& shaped : shapedTexts)
    {
        const auto& item = items[shaped.index];
        ProcessShapedText(atlas, shaped, item, buffers);
    }

    return CreatePageMeshes(m_meshProvider, buffers);
}