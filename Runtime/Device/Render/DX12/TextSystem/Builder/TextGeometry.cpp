#include "pch.h"
#include "TextGeometry.h"
#include "Definition/RenderData.h"
#include "../Atlas/FontAtlas.h"
#include "MeshBuilderHelpers.h"
#include "TextBatchInfo.h"
#include "UnderlineBatcher.h"
#include "GlyphCursor.h"
#include "TextLayoutHelper.h"

void AppendShapedText(
    const FontAtlas& atlas,
    const ShapedText& shaped,
    const RenderTextItem& item,
    TextBatchBufferMap& buffers)
{
    Rect clipRect = ComputeClipRect(item);

    std::vector<float> lineWidths = ComputeLineWidths(shaped.glyphs);
    GlyphCursor cursor{ shaped, item, lineWidths };
    UnderlineBatcher underline{ atlas, shaped, buffers, clipRect };
    const std::vector<PackedTextParams> runParams = PackRunParams(item.runs); // 런당 1회

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

        auto info = GetGlyphBatchInfo(buffers, glyph, atlas);
        const auto& params = runParams[shapedGlyph.runIndex];

        AppendGlyphQuad(
            info,
            *glyph,
            x,
            y,
            style.color,
            params,
            clipRect);

        cursor.Advance(shapedGlyph.advanceX);
    }

    underline.Flush(cursor.CursorX(), cursor.BaselineY());
}