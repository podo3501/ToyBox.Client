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

class TextSystem
{
public:
    ~TextSystem();
    TextSystem(TaskScheduler& taskScheduler, ResourceFactory& resFactory);
    bool Initialize(Device& device, DescriptorFactory& factory, const Size& atlasTexSize);
    std::vector<DrawUIItem> DrawText(
        std::shared_ptr<IMeshResource> meshRes,
        std::shared_ptr<IFontResource> fontRes,
        std::span<const char32_t> text,
        uint32_t size,
        const Core::Math::Vector2& pos,
        const Core::Color& color);

private:
    void UpdateAtlasIfNeeded(
        std::shared_ptr<IFontResource> fontRes,
        std::span<const char32_t> text,
        uint32_t size);
    const Resource& GetAtlasResource() const;

    Size m_atlasTextureSize{};
    std::shared_ptr<MaterialResource> m_matResource;
    
    GlyphCache m_glyphCache;
    AtlasPacker m_packer;

    FontAtlasCreateGraphBuilder m_atlasBuilder;
};
