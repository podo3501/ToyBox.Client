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
        ShadowResource& shadowRes) noexcept;

    void Build(
        RenderGraph& graph,
        RGResourceID shadowResID,
        const DirectionalLightData& light,
        std::vector<RenderShadowCasterItem> shadowCasters);

private:
    ShadowRenderer& m_shadowRenderer;
    DescriptorFactory& m_descFactory;
    ShadowResource& m_shadowRes;
};