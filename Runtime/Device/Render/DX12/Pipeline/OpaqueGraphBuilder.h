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
        SurfaceRenderer& surfRenderer, 
        SwapChainPresenter& swapChain,
        ShadowResource& shadowRes,
        RGHandle hBb, RGHandle hShadow);
    void Build(RenderGraph& graph);
    
private:
    SurfaceRenderer& m_surfRenderer;
    SwapChainPresenter& m_swapChain;
    ShadowResource&m_shadowRes;
    RGHandle m_hBb;
    RGHandle m_hShadow;
};