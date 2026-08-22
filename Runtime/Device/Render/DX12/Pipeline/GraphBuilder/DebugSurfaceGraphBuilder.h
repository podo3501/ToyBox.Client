#pragma once
#include "Graph/RGTypes.h"

struct ViewPacket;
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

    void Build(
        RenderGraph& graph,
        std::shared_ptr<ViewPacket> packet,
        size_t viewIndex);

private:
    DebugSurfaceRenderer& m_debugSurfRenderer;
    SwapChainPresenter& m_swapChain;
    RGResourceID m_backBufferResID;
};