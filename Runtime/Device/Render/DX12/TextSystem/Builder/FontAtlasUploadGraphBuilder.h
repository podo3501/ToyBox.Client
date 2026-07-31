#pragma once
#include "Resource/Resource.h"
#include "Graph/RGTypes.h"
#include "../TextTypes.h"

struct GlyphUploadLayout;
class TaskScheduler;
class ResourceFactory;
class RenderGraph;

class FontAtlasUploadGraphBuilder
{
public:
    ~FontAtlasUploadGraphBuilder();
    FontAtlasUploadGraphBuilder() = delete;
    FontAtlasUploadGraphBuilder(
        TaskScheduler& taskScheduler,
        ResourceFactory& resourceFactory);
    
    void UploadGlyphsToAtlas(
        const Resource& atlasResource,
        std::vector<GlyphUploadEntry> uploads);

private:
    void BuildUploadPass(
        RenderGraph& graph,
        RGResourceID atlasResID,
        RGResourceID uploadResID,
        std::vector<GlyphUploadEntry> uploads,
        std::vector<GlyphUploadLayout> layouts);

private:
    TaskScheduler& m_taskScheduler;
    ResourceFactory& m_resFactory;
};