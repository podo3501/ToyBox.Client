#pragma once
#include "Graph/RGTypes.h"

struct ViewPacket;
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

    void Build(
        RenderGraph& graph,
        std::shared_ptr<ViewPacket> packet,
        size_t viewIndex);

private:
    UIRenderer& m_uiRenderer;
    SwapChainPresenter& m_swapChain;
    RGResourceID m_backBufferResID;
};