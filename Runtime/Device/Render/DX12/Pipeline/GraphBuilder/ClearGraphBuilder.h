#pragma once
#include "Graph/RGTypes.h"

class RenderGraph;
class SwapChainPresenter;

class ClearGraphBuilder
{
public:
    ~ClearGraphBuilder();
    ClearGraphBuilder() = delete;
    ClearGraphBuilder(
        SwapChainPresenter& swapChain,
        RGResourceID backBufferResID);

    void Build(RenderGraph& graph);

private:
    SwapChainPresenter& m_swapChain;
    RGResourceID m_backBufferResID;
};