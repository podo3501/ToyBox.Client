#pragma once
#include "Graph/RGTypes.h"

class RenderGraph;
class DebugSurfaceRenderer;

class DebugSurfaceGraphBuilder
{
public:
    ~DebugSurfaceGraphBuilder();
    DebugSurfaceGraphBuilder() = delete;
    DebugSurfaceGraphBuilder(DebugSurfaceRenderer& debugSurfRenderer, RGHandle hBb);
    void Build(RenderGraph& graph);

private:
    DebugSurfaceRenderer& m_debugSurfRenderer;
    RGHandle m_hBb;
};