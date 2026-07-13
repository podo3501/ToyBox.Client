#pragma once
#include "GameClient/Service/Render/Resource/IFontResource.h"
#include "Resource/Material/UIMaterialResource.h"
#include "Core/RenderData.h"
#include "Core/Foundation/Color.h"
#include "GlyphCache.h"
#include "AtlasPacker.h"
#include "FontAtlasCreateGraphBuilder.h"

class Device;
class TaskScheduler;
class ResourceFactory;
class DescriptorFactory;
class TransientMeshResource;
class TransientMeshProvider;

struct ShapedGlyph;
struct ShapedText
{
    FontResource* font{};
    uint32_t size{};

    std::vector<ShapedGlyph> glyphs;
};

class TextSystem
{
public:
    ~TextSystem();
    TextSystem(
        TaskScheduler& taskScheduler, 
        ResourceFactory& resFactory,
        TransientMeshProvider& transientMeshProvider);
    bool Initialize(Device& device, DescriptorFactory& factory, const Size& atlasTexSize);
    std::vector<DrawUIItem> BuildDrawItems(std::span<const DrawTextItem> items);

private:
    std::shared_ptr<TransientMeshResource> CreateTextMesh(
        std::span<const DrawTextItem> items,
        std::span<const ShapedText> shapedTexts);
    void UpdateAtlasIfNeeded(
        const ShapedText& shapedText,
        std::vector<GlyphUploadEntry>& outUploads);
    const Resource& GetAtlasResource() const;

    Size m_atlasTextureSize{};
    std::shared_ptr<MaterialResource> m_matResource;
    
    GlyphCache m_glyphCache;
    AtlasPacker m_packer;
    FontAtlasCreateGraphBuilder m_atlasBuilder;
    TransientMeshProvider& m_transientMeshProvider;
};
