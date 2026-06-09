#pragma once
#include "RGTypes.h"

class RenderGraph;
class RenderScene;
class ShadowRenderer;
class DescriptorFactory;
class ShadowResource;

class ShadowGraphBuilder
{
public:
    ~ShadowGraphBuilder();
    ShadowGraphBuilder(
        ShadowRenderer* shadowRenderer, 
        DescriptorFactory* descFactory,
        ShadowResource* shadowRes,
        RenderScene* scene, RGHandle hShadow);
    void Build(RenderGraph& graph);

private:
    ShadowRenderer* m_shadowRenderer{ nullptr };
    DescriptorFactory* m_descFactory{ nullptr };
    ShadowResource* m_shadowRes{ nullptr };
    RenderScene* m_scene{ nullptr };
    RGHandle m_hShadow;
};