#pragma once
#include "Graph/RGTypes.h"

struct DirectionalLightData;
struct RenderShadowCasterItem;
class RenderGraph;
class ShadowRenderer;
class DescriptorFactory;
class ShadowResource;

class ShadowGraphBuilder
{
public:
    ~ShadowGraphBuilder();
    ShadowGraphBuilder(
        ShadowRenderer& shadowRenderer, 
        DescriptorFactory& descFactory,
        ShadowResource& shadowRes,
        RGResourceID shadowResID);

    void Build(
        RenderGraph& graph,
        const DirectionalLightData& light,
        std::vector<RenderShadowCasterItem> shadowCasters);

private:
    ShadowRenderer& m_shadowRenderer;
    DescriptorFactory& m_descFactory;
    ShadowResource& m_shadowRes;
    RGResourceID m_shadowResID;
};