#pragma once
#include "Graph/RGTypes.h"

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
    void Build(RenderGraph& graph);

private:
    ShadowRenderer& m_shadowRenderer;
    DescriptorFactory& m_descFactory;
    ShadowResource& m_shadowRes;
    RGResourceID m_shadowResID;
};