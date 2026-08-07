#pragma once
#include "Resource/Resource.h"
#include "Graph/RGTypes.h"
#include "../TextTypes.h"

struct GlyphUploadLayout;
struct ResourceContext;
class ResourceFactory;
class RenderGraph;

struct PendingAtlas
{
    RGResourceID rgID;
    std::vector<GlyphUploadEntry> glyphs;
};

class FontAtlasUploadGraphBuilder
{
public:
    ~FontAtlasUploadGraphBuilder();
    FontAtlasUploadGraphBuilder() = delete;
    explicit FontAtlasUploadGraphBuilder(ResourceFactory& resourceFactory);

    void QueueGlyphUploads(std::vector<AtlasGlyphBatch>&& batches);
    void Build(RenderGraph& graph);
    void ApplyResourceBindings(ResourceContext& resCtx) const;
    bool HasPendingUploads() const { return !m_pending.empty(); }

private:
    void BuildUploadPass(
        RenderGraph& graph,
        RGResourceID atlasResID,
        RGResourceID uploadResID,
        std::vector<GlyphUploadEntry> uploads,
        std::vector<GlyphUploadLayout> layouts);

private:
    ResourceFactory& m_resFactory;

    std::unordered_map<const Resource*, PendingAtlas> m_pending;
    std::unordered_map<RGResourceID, Resource> m_frameBindings;
};