#include "pch.h"
#include "TextSystem.h"
#include "Resource/Font/FontResource.h"
#include "Resource/Mesh/TransientMeshResource.h"
#include "Inspector/Inspector.h"
#include "GameClient/Service/Render/RenderConfig.h"
#include "TextLayout.h"

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

std::vector<DrawUIItem> TextSystem::BuildDrawItems(std::span<const DrawTextItem> items)
{
    if (items.empty())
        return {};

    auto shapedTexts = ShapeTexts(items);
    for (size_t i = 0; i < shapedTexts.size(); ++i)
        ApplyWordWrap(shapedTexts[i].glyphs, items[i].size.x, items[i].layout.wordWrap);
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

static std::vector<ShapedGlyph> ShapeRuns(
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
        shaped.glyphs = ShapeRuns(
            font,
            item.runs,
            item.fontSize);

        result.push_back(std::move(shaped));
    }

    return result;
}