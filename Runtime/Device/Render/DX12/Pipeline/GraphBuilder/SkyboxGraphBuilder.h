#pragma once
#include "Graph/RGTypes.h"

class RenderGraph;
class SkyboxRenderer;
class SwapChainPresenter;

class SkyboxGraphBuilder
{
public:
    ~SkyboxGraphBuilder();
    SkyboxGraphBuilder() = delete;
    SkyboxGraphBuilder(
        SkyboxRenderer& skyboxRenderer,
        SwapChainPresenter& swapChain,
        RGResourceID backBufferResID);

    void Build(RenderGraph& graph);

private:
    SkyboxRenderer& m_skyboxRenderer;
    SwapChainPresenter& m_swapChain;
    RGResourceID m_backBufferResID;
};
