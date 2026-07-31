#include "pch.h"
#include "TextMeshBuilder.h"
#include "../Atlas/FontAtlas.h"
#include "MeshBuilderHelpers.h"
#include "Provider/Mesh/TransientMeshProvider.h"
#include "Resource/Font/FontResource.h"
#include "TextBatch.h"
#include "UnderlineBatcher.h"
#include "GlyphCursor.h"
#include "TextLayoutHelper.h"

static void ProcessShapedText(
    const FontAtlas& atlas,
    const ShapedText& shaped,
    const DrawTextItem& item,
    TextBatchBufferMap& buffers)
{
    Rect clipRect = ComputeClipRect(item);

    std::vector<float> lineWidths = ComputeLineWidths(shaped.glyphs);
    GlyphCursor cursor{ shaped, item, lineWidths };
    UnderlineBatcher underline{ atlas, shaped, buffers, clipRect };

    uint32_t currentLine = UINT32_MAX;

    for (const auto& shapedGlyph : shaped.glyphs)
    {
        if (shapedGlyph.lineIndex != currentLine)
        {
            underline.Flush(cursor.CursorX(), cursor.BaselineY());
            currentLine = shapedGlyph.lineIndex;
            cursor.BeginLine(currentLine);
        }

        if (clipRect.IsValid() && !cursor.IsLineVisible(currentLine, clipRect))
            continue;

        const auto& style = item.runs[shapedGlyph.runIndex].style;
        underline.Update(style, cursor.CursorX(), cursor.BaselineY());

        const GlyphInfo* glyph = atlas.FindGlyph(
            shaped.font, shaped.mode, shapedGlyph.glyphIndex, shaped.size);
        if (!glyph || glyph->width == 0 || glyph->height == 0)
        {
            cursor.Advance(shapedGlyph.advanceX);
            continue;
        }

        float x = cursor.CursorX() + glyph->bearingX + shapedGlyph.offsetX;
        float y = cursor.BaselineY() - glyph->bearingY - shapedGlyph.offsetY;

        if (shaped.mode == TextRenderMode::Bitmap)
        {
            x = std::floor(x + 0.5f);
            y = std::floor(y + 0.5f);
        }

        if (clipRect.IsValid())
        {
            Rect glyphRect{ x, y, glyph->width, glyph->height };
            if (!glyphRect.Intersects(clipRect))
            {
                cursor.Advance(shapedGlyph.advanceX);
                continue;
            }
        }

        TextBatchKey key
        {
            glyph->bucketID,
            glyph->pageIndex
        };

        auto target = GetOrCreateBatchTarget(buffers, key, atlas.GetMaterial(glyph));
        AppendGlyphQuad(
            target.buffer.vertices,
            target.buffer.indices,
            target.buffer.vertexOffset,
            *glyph,
            x,
            y,
            target.texIndices[0],
            style,
            clipRect);

        cursor.Advance(shapedGlyph.advanceX);
    }

    underline.Flush(cursor.CursorX(), cursor.BaselineY());
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