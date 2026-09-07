#pragma once
#include "Graph/RGTypes.h"

struct Size;
struct ViewTarget;
struct ViewPacket;
struct ViewRenderOutput;
class RenderGraph;
class CompositeRenderer;
class SwapChainPresenter;

class CompositeGraphBuilder
{
public:
    CompositeGraphBuilder(
        CompositeRenderer& compositeRenderer,
        SwapChainPresenter& swapChain) noexcept;

    void Build(
        RenderGraph& graph, 
        RGResourceID backBufferResID,
        const std::vector<ViewRenderOutput>& viewOutputs);

private:
    CompositeRenderer& m_compositeRenderer;
    SwapChainPresenter& m_swapChain;
};