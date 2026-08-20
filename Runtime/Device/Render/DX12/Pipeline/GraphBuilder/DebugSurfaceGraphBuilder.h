#pragma once
#include "Graph/RGTypes.h"

class RenderGraph;
class DebugSurfaceRenderer;
class SwapChainPresenter;

class DebugSurfaceGraphBuilder
{
public:
    ~DebugSurfaceGraphBuilder();
    DebugSurfaceGraphBuilder() = delete;
    DebugSurfaceGraphBuilder(
        DebugSurfaceRenderer& debugSurfRenderer, 
        SwapChainPresenter& swapChain,
        RGResourceID backBufferResID);
    void Build(RenderGraph& graph);

private:
    DebugSurfaceRenderer& m_debugSurfRenderer;
    SwapChainPresenter& m_swapChain;
    RGResourceID m_backBufferResID;
};