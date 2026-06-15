#pragma once
#include "Graph/RGTypes.h"

class RenderGraph;
class SurfaceRenderer;
class SwapChainPresenter;
class ShadowResource;

class OpaqueGraphBuilder
{
public:
    ~OpaqueGraphBuilder();
    OpaqueGraphBuilder() = delete;
    OpaqueGraphBuilder(
        SurfaceRenderer* surfRenderer, 
        SwapChainPresenter* swapChain,
        ShadowResource* shadowRes,
        RGHandle hBb, RGHandle hShadow);
    void Build(RenderGraph& graph);
    
private:
    SurfaceRenderer* m_surfRenderer{ nullptr };
    SwapChainPresenter* m_swapChain{ nullptr };
    ShadowResource* m_shadowRes{ nullptr };
    RGHandle m_hBb;
    RGHandle m_hShadow;
};