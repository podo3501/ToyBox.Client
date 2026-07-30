#include "pch.h"
#include "TextMeshBuilder.h"
#include "../Atlas/FontAtlas.h"
#include "../TextHelpers.h"
#include "Provider/Mesh/TransientMeshProvider.h"
#include "Resource/Font/FontResource.h"
#include "TextBatch.h"
#include "UnderlineBatcher.h"

static float ComputeHorizontalOffset(
    TextHorizontalAlign align,
    float boundsWidth,
    float lineWidth)
{
    if (boundsWidth <= 0.f)
        return 0.f; // bounds width가 없으면 정렬 기준이 없으므로 Left와 동일

    switch (align)
    {
    case TextHorizontalAlign::Center: return (boundsWidth - lineWidth) * 0.5f;
    case TextHorizontalAlign::Right:  return (boundsWidth - lineWidth);
    case TextHorizontalAlign::Left:
    default: return 0.f;
    }
}

static float ComputeVerticalOffset(
    TextVerticalAlign align,
    float boundsHeight,
    float totalBlockHeight)
{
    if (boundsHeight <= 0.f)
        return 0.f; // bounds height가 없으면 정렬 기준이 없으므로 Top과 동일

    switch (align)
    {
    case TextVerticalAlign::Middle: return (boundsHeight - totalBlockHeight) * 0.5f;
    case TextVerticalAlign::Bottom: return (boundsHeight - totalBlockHeight);
    case TextVerticalAlign::Top:
    default: return 0.f;
    }
}

static std::vector<float> ComputeLineWidths(std::span<const ShapedGlyph> glyphs)
{
    std::vector<float> lineWidths;
    for (const auto& g : glyphs)
    {
        if (g.lineIndex >= lineWidths.size())
            lineWidths.resize(g.lineIndex + 1, 0.f);
        lineWidths[g.lineIndex] += g.advanceX;
    }
    return lineWidths;
}

static void ProcessShapedText(
    const FontAtlas& atlas,
    const ShapedText& shaped,
    const DrawTextItem& item,
    TextBatchBufferMap& buffers)
{
    const bool clip = (item.layout.overflow == TextOverflow::Clip);

    float baseLineHeight = shaped.font->GetLineHeight(shaped.size);
    float lineHeight = baseLineHeight * item.layout.lineSpacing;
    float ascent = shaped.font->GetAscent(shaped.size);

    std::vector<float> lineWidths = ComputeLineWidths(shaped.glyphs);

    float totalBlockHeight = static_cast<float>(lineWidths.size()) * lineHeight;
    float verticalOffset = ComputeVerticalOffset(item.layout.verticalAlign, item.size.y, totalBlockHeight);
    float baseY = item.position.y + verticalOffset;

    uint32_t currentLine = UINT32_MAX;
    float cursorX = item.position.x;
    float baselineY = baseY + ascent;

    UnderlineBatcher underline{ atlas, shaped, buffers };

    for (const auto& shapedGlyph : shaped.glyphs)
    {
        if (shapedGlyph.lineIndex != currentLine)
        {
            underline.Flush(cursorX, baselineY); // 줄이 바뀌면 이전 줄 구간을 닫음

            currentLine = shapedGlyph.lineIndex;
            float lineWidth = (currentLine < lineWidths.size()) ? lineWidths[currentLine] : 0.f;
            float alignOffset = ComputeHorizontalOffset(item.layout.horizontalAlign, item.size.x, lineWidth);

            cursorX = item.position.x + alignOffset;
            baselineY = baseY + ascent + currentLine * lineHeight;
        }

        if (clip && item.size.y > 0.f)
        {
            float lineTop = verticalOffset + currentLine * lineHeight;
            float lineBottom = lineTop + lineHeight;
            if (lineTop < 0.f || lineBottom > item.size.y)
                continue; // 줄 전체가 완전히 들어가지 않으면 스킵
        }

        const auto& style = item.runs[shapedGlyph.runIndex].style;
        underline.Update(style, cursorX, baselineY);

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

        // width 클리핑: 박스를 넘어가면 스킵
        if (clip && item.size.x > 0.f)
        {
            float localLeft = cursorX - item.position.x;
            float localRight = localLeft + glyph->width;
            if (localLeft < 0.f || localRight > item.size.x)
            {
                cursorX += shapedGlyph.advanceX;
                continue;
            }
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

    underline.Flush(cursorX, baselineY); // 마지막 구간
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