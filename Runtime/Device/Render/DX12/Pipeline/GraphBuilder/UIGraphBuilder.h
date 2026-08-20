#pragma once
#include "Graph/RGTypes.h"

class RenderGraph;
class UIRenderer;
class SwapChainPresenter;

class UIGraphBuilder
{
public:
    ~UIGraphBuilder();
    UIGraphBuilder() = delete;
    UIGraphBuilder(
        UIRenderer& uiRenderer, 
        SwapChainPresenter& swapChain, 
        RGResourceID backBufferResID);

    void Build(RenderGraph& graph);

private:
    UIRenderer& m_uiRenderer;
    SwapChainPresenter& m_swapChain;
    RGResourceID m_backBufferResID;
};