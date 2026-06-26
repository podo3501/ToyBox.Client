#pragma once
#include "Graph/RGTypes.h"

class RenderGraph;
class DebugSurfaceRenderer;

class DebugSurfaceGraphBuilder
{
public:
    ~DebugSurfaceGraphBuilder();
    DebugSurfaceGraphBuilder() = delete;
    DebugSurfaceGraphBuilder(DebugSurfaceRenderer& debugSurfRenderer, RGResourceID backBufferResID);
    void Build(RenderGraph& graph);

private:
    DebugSurfaceRenderer& m_debugSurfRenderer;
    RGResourceID m_backBufferResID;
};