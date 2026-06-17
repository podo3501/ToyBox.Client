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
        ShadowRenderer* shadowRenderer, 
        DescriptorFactory& descFactory,
        ShadowResource* shadowRes,
        RGHandle hShadow);
    void Build(RenderGraph& graph);

private:
    ShadowRenderer* m_shadowRenderer{ nullptr };
    DescriptorFactory& m_descFactory;
    ShadowResource* m_shadowRes{ nullptr };
    RGHandle m_hShadow;
};