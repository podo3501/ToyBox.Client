#pragma once
#include "RGTypes.h"

class RenderGraph;
class RenderScene;
class SurfaceRenderer;
class SwapChainPresenter;

class OpaqueGraphBuilder
{
public:
    ~OpaqueGraphBuilder();
    OpaqueGraphBuilder() = delete;
    OpaqueGraphBuilder(
        SurfaceRenderer* surfRenderer, 
        SwapChainPresenter* swapChain,
        RenderScene* scene, 
        RGHandle hBb, RGHandle hShadow);
    void Build(RenderGraph& graph);
    
private:
    SurfaceRenderer* m_surfRenderer{ nullptr };
    SwapChainPresenter* m_swapChain{ nullptr };
    RenderScene* m_scene{ nullptr };
    RGHandle m_hBb;
    RGHandle m_hShadow;
};