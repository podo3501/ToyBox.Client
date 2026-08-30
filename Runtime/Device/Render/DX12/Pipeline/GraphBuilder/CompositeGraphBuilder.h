#pragma once
#include "Graph/RGTypes.h"

struct Size;
struct ViewTarget;
struct ViewPacket;
struct RenderViewInfo;
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
        const std::vector<RenderViewInfo>& renderViewInfos);

private:
    CompositeRenderer& m_compositeRenderer;
    SwapChainPresenter& m_swapChain;
    RGResourceID m_backBufferResID;
};