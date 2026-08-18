#include "pch.h"
#include "TextLayout.h"
#include "Core/RenderData.h"
#include "Resource/Font/FontResource.h"

static bool IsCJK(char32_t cp)
{
    return (cp >= 0x1100 && cp <= 0x11FF)   // 한글 자모 (조합형)
        || (cp >= 0x3040 && cp <= 0x30FF)   // 히라가나 + 가타카나
        || (cp >= 0x3130 && cp <= 0x318F)   // 한글 호환 자모
        || (cp >= 0x3400 && cp <= 0x4DBF)   // CJK 확장 한자 A
        || (cp >= 0x4E00 && cp <= 0x9FFF)   // CJK 통합 한자
        || (cp >= 0xAC00 && cp <= 0xD7A3)   // 한글 음절 (가~힣)
        || (cp >= 0xF900 && cp <= 0xFAFF);  // CJK 호환용 한자
}

static bool IsWhitespace(char32_t cp)
{
    return cp == U' ' || cp == U'\t';
}

struct WrapState
{
    uint32_t currentLine;
    float lineWidth;
    std::optional<size_t> lastBreak;
};

// [breakAt, i] 구간을 새 줄로 시작한다. breakAt이 공백이면 그 공백은 어느 줄의 너비 계산에도 넣지 않고 건너뛴다.
static void StartNewLine(
    std::span<ShapedGlyph> glyphs,
    size_t breakAt, size_t i,
    WrapState& state)
{
    if (IsWhitespace(glyphs[breakAt].codepoint))
    {
        glyphs[breakAt].lineIndex = state.currentLine;
        ++breakAt;
    }

    ++state.currentLine;
    state.lineWidth = 0.f;
    for (size_t k = breakAt; k <= i; ++k)
    {
        glyphs[k].lineIndex = state.currentLine;
        state.lineWidth += glyphs[k].advanceX;
    }

    state.lastBreak.reset();
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

    WrapState state{ baseLineIndex, 0.f, std::nullopt };

    for (size_t i = start; i < end; ++i)
    {
        if (IsWhitespace(glyphs[i].codepoint) || IsCJK(glyphs[i].codepoint))
            state.lastBreak = i;

        float glyphWidth = glyphs[i].advanceX;

        // 브레이크 지점이 없고, 이 glyph 하나만으로도 줄이 넘치는 경우 -> 문자 단위 강제 컷
        if (state.lineWidth + glyphWidth > maxWidth && i > start && !state.lastBreak.has_value())
        {
            StartNewLine(glyphs, i, i, state);
            continue;
        }

        state.lineWidth += glyphWidth;

        if (state.lineWidth > maxWidth && i > start)
        {
            size_t breakAt = state.lastBreak.value_or(i);
            StartNewLine(glyphs, breakAt, i, state);
            continue;
        }

        glyphs[i].lineIndex = state.currentLine;
    }

    return state.currentLine;
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

        lineOffset = lastLine - hardLine;
        start = end;
    }
}

std::vector<ShapedGlyph> ShapeRuns(
    FontResource* font,
    std::span<const TextRun> runs,
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
            auto it = std::ranges::upper_bound(runStarts, glyph.sourceIndex);
            uint32_t localRun = static_cast<uint32_t>(std::distance(runStarts.begin(), it) - 1);

            glyph.runIndex = static_cast<uint32_t>(groupStart + localRun);
            glyph.lineIndex = lineIndex;
            Assert(glyph.sourceIndex < combined.size()); //font->Shape()가 유효 범위를 벗어난 sourceIndex를 반환
            glyph.codepoint = (glyph.sourceIndex < combined.size())
                ? combined[glyph.sourceIndex]
                : U'\0';

            result.push_back(std::move(glyph));
        }
    }

    return result;
}