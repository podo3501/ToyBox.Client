#include "pch.h"
#include "TextSystem.h"
#include "Resource/Font/FontResource.h"
#include "Resource/Mesh/TransientMeshResource.h"
#include "Inspector/Inspector.h"
#include "GameClient/Service/Render/RenderConfig.h"

TextSystem::~TextSystem() = default;
TextSystem::TextSystem(
    Device& device,
    DescriptorFactory& factory,
    TaskScheduler& taskScheduler, 
    ResourceFactory& resFactory,
    TransientMeshProvider& transientMeshProvider) :
    m_atlasBuilder{ taskScheduler, resFactory },
    m_meshBuilder{ transientMeshProvider },
    m_fontAtlas{ device, factory, m_atlasBuilder }
{}

bool TextSystem::Initialize(const TextConfig& texConfig, Inspector* inspector)
{
    ReturnIfFalse(m_fontAtlas.Initialize(texConfig));
   m_inspector = inspector;

   return true;
}

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

static void ApplyWordWrap(std::span<ShapedGlyph> glyphs, float maxWidth)
{
    if (glyphs.empty())
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

std::vector<DrawUIItem> TextSystem::BuildDrawItems(std::span<const DrawTextItem> items)
{
    if (items.empty())
        return {};

    auto shapedTexts = ShapeTexts(items);
    for (size_t i = 0; i < shapedTexts.size(); ++i)
        ApplyWordWrap(shapedTexts[i].glyphs, items[i].size.x);
    m_fontAtlas.EnsureGlyphs(shapedTexts);

    std::vector<PageMesh> pageMeshes = m_meshBuilder.Build(m_fontAtlas, items, shapedTexts);
    return CreateDrawItems(pageMeshes);
}

std::vector<DrawUIItem> TextSystem::CreateDrawItems(std::span<const PageMesh> pageMeshes)
{
    std::vector<DrawUIItem> result;
    result.reserve(pageMeshes.size());

    for (const auto& pageMesh : pageMeshes)
    {
        DrawUIItem item;
        item.mesh = pageMesh.mesh;
        item.material = pageMesh.material;

        result.push_back(std::move(item));
    }

    return result;
}

std::vector<ShapedText> TextSystem::ShapeTexts(std::span<const DrawTextItem> items)
{
    std::vector<ShapedText> result;
    result.reserve(items.size());

    for (size_t index = 0; index < items.size(); ++index)
    {
        const auto& item = items[index];

        Assert(item.fontRes);
        auto font = static_cast<FontResource*>(item.fontRes.get());

        ShapedText shaped;
        shaped.font = font;
        shaped.mode = item.mode;
        shaped.size = item.fontSize;
        shaped.index = index;

        std::vector<ShapedGlyph> allGlyphs;

        size_t runIdx = 0;
        while (runIdx < item.runs.size())
        {
            uint32_t curLine = item.runs[runIdx].lineIndex;
            size_t groupStart = runIdx;

            std::vector<char32_t> combined;
            std::vector<size_t> runStarts;
            while (runIdx < item.runs.size() && item.runs[runIdx].lineIndex == curLine)
            {
                runStarts.push_back(combined.size());
                combined.insert(combined.end(),
                    item.runs[runIdx].codePoints.begin(),
                    item.runs[runIdx].codePoints.end());
                ++runIdx;
            }

            auto glyphs = font->Shape(combined, item.fontSize);
            for (auto& glyph : glyphs)
            {
                uint32_t localRunIdx = 0;
                for (; localRunIdx + 1 < runStarts.size(); ++localRunIdx)
                    if (glyph.sourceIndex < runStarts[localRunIdx + 1])
                        break;

                glyph.runIndex = static_cast<uint32_t>(groupStart + localRunIdx);
                glyph.lineIndex = curLine;
                glyph.codepoint = combined[glyph.sourceIndex];
                allGlyphs.push_back(glyph);
            }
        }

        shaped.glyphs = std::move(allGlyphs);
        result.push_back(std::move(shaped));
    }

    return result;
}