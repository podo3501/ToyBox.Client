#pragma once
#include "RGTypes.h"

class RenderGraph;
class RenderScene;
class SurfaceRenderer;

class OpaqueGraphBuilder
{
public:
    ~OpaqueGraphBuilder();
    OpaqueGraphBuilder() = delete;
    OpaqueGraphBuilder(SurfaceRenderer* surfRenderer, RenderScene* scene, RGHandle hBb);
    void Build(RenderGraph& graph);
    
private:
    SurfaceRenderer* m_surfRenderer{ nullptr };
    RenderScene* m_scene{ nullptr };
    RGHandle m_hBb;
};