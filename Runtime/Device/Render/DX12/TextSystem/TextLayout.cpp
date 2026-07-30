#include "pch.h"
#include "TextLayout.h"

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
    size_t start,
    size_t end,
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
    size_t lineStart = start;

    while (lineStart < end)
    {
        float lineWidth = 0.f;
        int lastBreak = -1;

        size_t i = lineStart;

        // 현재 줄에 들어갈 glyph 범위 탐색
        for (; i < end; ++i)
        {
            const auto& glyph = glyphs[i];

            if (IsWhitespace(glyph.codepoint) || IsCJK(glyph.codepoint))
                lastBreak = static_cast<int>(i);

            float nextWidth = lineWidth + glyph.advanceX;

            if (nextWidth > maxWidth && i > lineStart)
                break;

            lineWidth = nextWidth;
        }


        // 남은 모든 glyph가 현재 줄에 들어가는 경우
        if (i == end)
        {
            for (size_t k = lineStart; k < end; ++k)
                glyphs[k].lineIndex = currentLine;

            break;
        }


        size_t nextLineStart = i;


        // 끊을 지점이 있는 경우
        if (lastBreak >= static_cast<int>(lineStart))
        {
            size_t breakAt = static_cast<size_t>(lastBreak);

            for (size_t k = lineStart; k <= breakAt; ++k)
                glyphs[k].lineIndex = currentLine;

            nextLineStart = breakAt + 1;
        }
        else
        {
            // 공백 없는 긴 단어:
            // 현재 glyph부터 다음 줄로 강제 이동
            for (size_t k = lineStart; k < i; ++k)
                glyphs[k].lineIndex = currentLine;
        }


        ++currentLine;
        lineStart = nextLineStart;
    }

    return currentLine;
}

//static uint32_t ApplyWordWrapToRange(
//    std::span<ShapedGlyph> glyphs,
//    size_t start, size_t end,
//    float maxWidth,
//    uint32_t baseLineIndex)
//{
//    if (maxWidth <= 0.f)
//    {
//        for (size_t i = start; i < end; ++i)
//            glyphs[i].lineIndex = baseLineIndex;
//        return baseLineIndex;
//    }
//
//    uint32_t currentLine = baseLineIndex;
//    float lineWidth = 0.f;
//    int lastBreak = -1;
//
//    for (size_t i = start; i < end; ++i)
//    {
//        if (IsWhitespace(glyphs[i].codepoint) || IsCJK(glyphs[i].codepoint))
//            lastBreak = static_cast<int>(i);
//
//        float glyphWidth = glyphs[i].advanceX;
//
//        // 브레이크 지점이 없고, 이 glyph 하나만으로도 줄이 넘치는 경우 -> 문자 단위 강제 컷
//        if (lineWidth + glyphWidth > maxWidth && i > start && lastBreak < static_cast<int>(start))
//        {
//            ++currentLine;
//            lineWidth = 0.f;
//            lastBreak = -1;
//            glyphs[i].lineIndex = currentLine;
//            lineWidth += glyphWidth;
//            continue;
//        }
//
//        lineWidth += glyphWidth;
//
//        if (lineWidth > maxWidth && i > start)
//        {
//            size_t breakAt = (lastBreak >= 0) ? static_cast<size_t>(lastBreak) : i;
//
//            ++currentLine;
//            lineWidth = 0.f;
//            for (size_t k = breakAt; k <= i; ++k)
//            {
//                glyphs[k].lineIndex = currentLine;
//                lineWidth += glyphs[k].advanceX;
//            }
//            lastBreak = -1;
//            continue;
//        }
//
//        glyphs[i].lineIndex = currentLine;
//    }
//
//    return currentLine;
//}

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
