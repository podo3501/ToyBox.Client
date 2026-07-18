#include "pch.h"
#include "TextSystem.h"
#include "Resource/Font/FontResource.h"
#include "Resource/Mesh/TransientMeshResource.h"

TextSystem::~TextSystem() = default;
TextSystem::TextSystem(
    Device& device,
    DescriptorFactory& factory,
    TaskScheduler& taskScheduler, 
    ResourceFactory& resFactory,
    TransientMeshProvider& transientMeshProvider) :
    m_fontAtlas{ device, factory },
    m_atlasBuilder{ taskScheduler, resFactory },
    m_meshBuilder{ transientMeshProvider }
{}

bool TextSystem::Initialize(const Size& atlasTexSize)
{
    return m_fontAtlas.Initialize(atlasTexSize);
}

std::vector<DrawUIItem> TextSystem::BuildDrawItems(std::span<const DrawTextItem> items)
{
    if (items.empty())
        return {};

    auto shapedTexts = ShapeTexts(items);
    UploadPendingGlyphs(shapedTexts);

    auto pageMeshes = m_meshBuilder.Build(m_fontAtlas, items, shapedTexts);
    return CreateDrawItems(pageMeshes);
}

void TextSystem::UploadPendingGlyphs(std::span<const ShapedText> shapedTexts)
{
    std::unordered_map<FontBucketID, FontAtlasUpload> uploads;
    
    for (const auto& shaped : shapedTexts)
        m_fontAtlas.EnsureGlyphs(shaped, uploads);

    for (auto& [bucket, upload] : uploads)
    {
        for (uint16_t page = 0; page < upload.pages.size(); ++page)
        {
            auto& glyphs = upload.pages[page];
            if (glyphs.empty())
                continue;

            auto& atlasRes = m_fontAtlas.GetAtlasResource(bucket, page);
            m_atlasBuilder.UploadGlyphsToAtlas(atlasRes, glyphs);
        }
    }
}

std::vector<DrawUIItem> TextSystem::CreateDrawItems(std::span<const PageMesh> pageMeshes)
{
    std::vector<DrawUIItem> result;
    result.reserve(pageMeshes.size());

    for (const auto& pageMesh : pageMeshes)
    {
        DrawUIItem item;
        item.mesh = pageMesh.mesh;
        item.material = m_fontAtlas.GetMaterial(pageMesh.bucketID, pageMesh.pageIndex);

        result.push_back(std::move(item));
    }

    return result;
}

std::vector<ShapedText> TextSystem::ShapeTexts(std::span<const DrawTextItem> items)
{
    std::vector<ShapedText> result;
    result.reserve(items.size());

    for (const auto& item : items)
    {
        Assert(item.fontRes);
        auto font = static_cast<FontResource*>(item.fontRes.get());

        ShapedText shaped;
        shaped.font = font;
        shaped.size = item.fontSize;
        shaped.glyphs = font->Shape(item.codePoints, item.fontSize);

        result.push_back(std::move(shaped));
    }

    return result;
}
