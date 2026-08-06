#pragma once
#include "Resource/Resource.h"
#include "Graph/RGTypes.h"
#include "../TextTypes.h"
#include "GlyphRegistry.h"

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

    void BuildFinalizePass(
        RenderGraph& graph,
        RGResourceID atlasResID,
        std::vector<RGResourceID> readyResIDs);

private:
    TaskScheduler& m_taskScheduler;
    ResourceFactory& m_resFactory;
    GlyphRegistry m_registry;
};