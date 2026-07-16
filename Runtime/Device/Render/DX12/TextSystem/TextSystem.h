#pragma once
#include "AtlasPage.h"
#include "GlyphCache.h"
#include "FontAtlasCreateGraphBuilder.h"
#include "GameClient/Service/Render/Resource/IFontResource.h"
#include "Resource/Material/UIMaterialResource.h"
#include "Core/RenderData.h"
#include "AtlasPacker.h"

struct PageMesh;
class Device;
class TaskScheduler;
class ResourceFactory;
class DescriptorFactory;
class TransientMeshResource;
class TransientMeshProvider;

class TextSystem
{
public:
    ~TextSystem();
    TextSystem(
        Device& device,
        DescriptorFactory& factory,
        TaskScheduler& taskScheduler, 
        ResourceFactory& resFactory,
        TransientMeshProvider& transientMeshProvider);
    bool Initialize(const Size& atlasTexSize);
    std::vector<DrawUIItem> BuildDrawItems(std::span<const DrawTextItem> items);

private:
    void CreatePage();
    std::vector<ShapedText> ShapeTexts(std::span<const DrawTextItem> items);
    std::vector<PageMesh> CreateTextMesh(
        std::span<const DrawTextItem> items,
        std::span<const ShapedText> shapedTexts);
    void EnsureGlyphs(
        const ShapedText& shapedText,
        std::vector<std::vector<GlyphUploadEntry>>& outUploadsPerPage);
    uint16_t CurrentPageIndex() const;

    Device& m_device;
    DescriptorFactory& m_factory;

    Size m_atlasTextureSize{};
    GlyphCache m_glyphCache;
    
    FontAtlasCreateGraphBuilder m_atlasBuilder;
    TransientMeshProvider& m_transientMeshProvider;

    std::vector<std::unique_ptr<AtlasPage>> m_pages;
    std::unique_ptr<AtlasPage> m_secondPage;
};
