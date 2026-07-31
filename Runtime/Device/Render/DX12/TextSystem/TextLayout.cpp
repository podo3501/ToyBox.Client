#include "pch.h"
#include "TextLayout.h"
#include "Core/RenderData.h"
#include "Resource/Font/FontResource.h"

static bool IsCJK(char32_t cp)
{
    return (cp >= 0xAC00 && cp <= 0xD7A3)
        || (cp >= 0x3130 && cp <= 0x318F)
        || (cp >= 0x4E00 && cp <= 0x9FFF)
        || (cp >= 0x3040 && cp <= 0x30FF);
}

static bool IsWhitespace(char32_t cp)
{
    return cp == U' ' || cp == U'\t';
}

static uint32_t ApplyWordWrapToRange(
    std::span<ShapedGlyph> glyphs,
    size_t start, size_t end,
    float maxWidth,
    uint32_t baseLineIndex)
{
    if (maxWidth <= 0.f)
    {
        for (size_t i = start; i < end; ++i)
            glyphs[i].lineIndex = baseLineIndex;
        return baseLineIndex;
    }

    uint32_t currentLine = baseLineIndex;
    float lineWidth = 0.f;
    int lastBreak = -1;

    for (size_t i = start; i < end; ++i)
    {
        if (IsWhitespace(glyphs[i].codepoint) || IsCJK(glyphs[i].codepoint))
            lastBreak = static_cast<int>(i);

        float glyphWidth = glyphs[i].advanceX;

        // 브레이크 지점이 없고, 이 glyph 하나만으로도 줄이 넘치는 경우 -> 문자 단위 강제 컷
        if (lineWidth + glyphWidth > maxWidth && i > start && lastBreak < static_cast<int>(start))
        {
            ++currentLine;
            lineWidth = 0.f;
            lastBreak = -1;
            glyphs[i].lineIndex = currentLine;
            lineWidth += glyphWidth;
            continue;
        }

        lineWidth += glyphWidth;

        if (lineWidth > maxWidth && i > start)
        {
            size_t breakAt = (lastBreak >= 0) ? static_cast<size_t>(lastBreak) : i;

            ++currentLine;
            lineWidth = 0.f;
            for (size_t k = breakAt; k <= i; ++k)
            {
                glyphs[k].lineIndex = currentLine;
                lineWidth += glyphs[k].advanceX;
            }
            lastBreak = -1;
            continue;
        }

        glyphs[i].lineIndex = currentLine;
    }

    return currentLine;
}

void ApplyWordWrap(std::span<ShapedGlyph> glyphs, float maxWidth, bool wordWrap)
{
    if (!wordWrap || glyphs.empty())
        return;

    size_t start = 0;
    uint32_t lineOffset = 0; // wrap으로 늘어난 만큼 뒤 hard-line들을 밀어줌

    while (start < glyphs.size())
    {
        uint32_t hardLine = glyphs[start].lineIndex;
        size_t end = start;
        while (end < glyphs.size() && glyphs[end].lineIndex == hardLine)
            ++end;

        uint32_t lastLine = ApplyWordWrapToRange(
            glyphs, start, end, maxWidth, hardLine + lineOffset);

        lineOffset += (lastLine - (hardLine + lineOffset));
        start = end;
    }
}

std::vector<ShapedGlyph> ShapeRuns(
    FontResource* font,
    std::span<const DrawTextRun> runs,
    uint32_t fontSize)
{
    std::vector<ShapedGlyph> result;

    size_t runIdx = 0;

    while (runIdx < runs.size())
    {
        uint32_t lineIndex = runs[runIdx].lineIndex;
        size_t groupStart = runIdx;

        std::vector<char32_t> combined;
        std::vector<size_t> runStarts;

        while (runIdx < runs.size() &&
            runs[runIdx].lineIndex == lineIndex)
        {
            runStarts.push_back(combined.size());

            combined.insert(
                combined.end(),
                runs[runIdx].codePoints.begin(),
                runs[runIdx].codePoints.end());

            ++runIdx;
        }

        auto glyphs = font->Shape(combined, fontSize);

        for (auto& glyph : glyphs)
        {
            uint32_t localRun = 0;
            for (; localRun + 1 < runStarts.size(); ++localRun)
            {
                if (glyph.sourceIndex < runStarts[localRun + 1])
                    break;
            }

            glyph.runIndex = static_cast<uint32_t>(groupStart + localRun);
            glyph.lineIndex = lineIndex;
            glyph.codepoint = combined[glyph.sourceIndex];

            result.push_back(std::move(glyph));
        }
    }

    return result;
}