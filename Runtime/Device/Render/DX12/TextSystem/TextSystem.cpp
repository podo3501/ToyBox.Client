#include "pch.h"
#include "TextSystem.h"
#include "Resource/Font/FontResource.h"
#include "Resource/Mesh/TransientMeshResource.h"
#include "Inspector/Inspector.h"
#include "GameClient/Service/Render/RenderConfig.h"

struct ShapedTextGroup
{
    std::array<std::vector<ShapedText>, Core::EnumSize<TextRenderMode>> texts;
};

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

    auto shapedGroups = ShapeTexts(items);
    for (size_t i = 0; i < shapedGroups.texts.size(); ++i)
    {
        auto& shapedTexts = shapedGroups.texts[i];
        if (shapedTexts.empty())
            continue;

        auto mode = static_cast<TextRenderMode>(i);
        m_fontAtlas.EnsureGlyphs(mode, shapedTexts);
    }

    std::vector<PageMesh> pageMeshes;
    for (size_t i = 0; i < shapedGroups.texts.size(); ++i)
    {
        auto& shapedTexts = shapedGroups.texts[i];
        if (shapedTexts.empty())
            continue;

        auto meshes = m_meshBuilder.Build(m_fontAtlas, items, shapedTexts);
        pageMeshes.insert(
            pageMeshes.end(),
            std::make_move_iterator(meshes.begin()),
            std::make_move_iterator(meshes.end()));
    }

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

ShapedTextGroup TextSystem::ShapeTexts(std::span<const DrawTextItem> items)
{
    ShapedTextGroup result;

    for (size_t index = 0; index < items.size(); ++index)
    {
        const auto& item = items[index];

        Assert(item.fontRes);
        auto font = static_cast<FontResource*>(item.fontRes.get());

        ShapedText shaped;
        shaped.font = font;
        shaped.mode = item.style.mode;
        shaped.size = item.fontSize;
        shaped.index = index;
        shaped.glyphs = font->Shape(
            item.codePoints,
            item.fontSize);

        result.texts[Core::ToIndex(item.style.mode)].push_back(std::move(shaped));
    }

    return result;
}