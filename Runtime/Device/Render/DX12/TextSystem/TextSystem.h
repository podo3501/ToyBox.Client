#pragma once
#include "GameClient/Service/Render/Resource/IFontResource.h"
#include "Resource/Material/UIMaterialResource.h"
#include "Core/RenderData.h"
#include "GlyphCache.h"
#include "AtlasPacker.h"
#include "Resource/Resource.h"
#include "Graph/RGTypes.h"


struct GlyphUploadEntry;
struct TextRenderLayout;
class Device;
class TaskScheduler;
class ResourceFactory;
class DescriptorFactory;
class RenderGraph;

class TextSystem
{
public:
    ~TextSystem();
    TextSystem(TaskScheduler& taskScheduler, ResourceFactory& resFactory);
    bool Initialize(Device& device, DescriptorFactory& factory, const Size& atlasTexSize);
    std::vector<DrawUIItem> DrawText(
        std::shared_ptr<IMeshResource> meshRes,
        std::shared_ptr<IFontResource> fontRes,
        std::string_view text,
        uint32_t size,
        const Core::Math::Vector2& pos);

private:
    void UpdateAtlasIfNeeded(
        std::shared_ptr<IFontResource> fontRes,
        std::string_view text,
        uint32_t size);
    void UploadGlyphsToAtlas(const std::vector<GlyphUploadEntry>& uploads);
    void BuildUploadPass(
        RenderGraph& graph, 
        RGResourceID atlasResID, 
        RGResourceID uploadResID, 
        const std::vector<GlyphUploadEntry>& uploads,
        const std::vector<TextRenderLayout>& layouts);
    const Resource& GetAtlasResource() const;

    Size m_atlasTextureSize{};
    std::shared_ptr<MaterialResource> m_matResource;
    
    GlyphCache m_glyphCache;
    AtlasPacker m_packer;

    //builder로 갈 변수들
    TaskScheduler& m_taskScheduler;
    ResourceFactory& m_resFactory;
};
