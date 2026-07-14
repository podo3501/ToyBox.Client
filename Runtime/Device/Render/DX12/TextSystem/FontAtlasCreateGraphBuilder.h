#pragma once
#include "Resource/Resource.h"
#include "Graph/RGTypes.h"
#include "TextTypes.h"

struct TextRenderLayout;
class TaskScheduler;
class ResourceFactory;
class RenderGraph;

class FontAtlasCreateGraphBuilder
{
public:
    ~FontAtlasCreateGraphBuilder();
    FontAtlasCreateGraphBuilder() = delete;
    FontAtlasCreateGraphBuilder(
        TaskScheduler& taskScheduler,
        ResourceFactory& resourceFactory);
    
    void UploadGlyphsToAtlas(
        const Resource& atlasResource,
        const std::vector<GlyphUploadEntry>& uploads);

private:
    void BuildUploadPass(
        RenderGraph& graph,
        RGResourceID atlasResID,
        RGResourceID uploadResID,
        const std::vector<GlyphUploadEntry>& uploads,
        const std::vector<TextRenderLayout>& layouts);

private:
    TaskScheduler& m_taskScheduler;
    ResourceFactory& m_resFactory;
};