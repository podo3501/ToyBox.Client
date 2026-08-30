#pragma once
#include "Graph/RGTypes.h"

class RenderGraph;
class SwapChainPresenter;

class ClearGraphBuilder
{
public:
    ~ClearGraphBuilder();
    ClearGraphBuilder() = delete;
    explicit ClearGraphBuilder(SwapChainPresenter& swapChain) noexcept;

    void Build(RenderGraph& graph, RGResourceID backBufferResID);

private:
    SwapChainPresenter& m_swapChain;
};