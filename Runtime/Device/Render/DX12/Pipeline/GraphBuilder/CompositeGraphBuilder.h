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
        SwapChainPresenter& swapChain,
        RGResourceID backBufferResID);

    void Build(
        RenderGraph& graph, 
        const std::vector<RenderViewInfo>& renderViewInfos);

private:
    CompositeRenderer& m_compositeRenderer;
    SwapChainPresenter& m_swapChain;
    RGResourceID m_backBufferResID;
};