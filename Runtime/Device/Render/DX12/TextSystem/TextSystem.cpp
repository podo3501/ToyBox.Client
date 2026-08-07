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
    ResourceFactory& resFactory,
    TransientMeshProvider& transientMeshProvider) :
    m_atlasBuilder{ resFactory },
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