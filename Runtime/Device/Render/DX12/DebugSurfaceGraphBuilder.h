#pragma once
#include "Graph/RGTypes.h"

class RenderGraph;
class RenderScene;
class DebugSurfaceRenderer;

class DebugSurfaceGraphBuilder
{
public:
    ~DebugSurfaceGraphBuilder();
    DebugSurfaceGraphBuilder() = delete;
    DebugSurfaceGraphBuilder(DebugSurfaceRenderer* debugSurfRenderer, RenderScene* scene, RGHandle hBb);
    void Build(RenderGraph& graph);

private:
    DebugSurfaceRenderer* m_debugSurfRenderer{ nullptr };
    RenderScene* m_scene{ nullptr };
    RGHandle m_hBb;
};